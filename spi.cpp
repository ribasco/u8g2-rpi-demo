#include <cstdint>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstdio>
#include <iostream>
#include "spi.h"

using namespace std;

#define errstr(errno) string(strerror(errno))

static const char *spiDev0 = "/dev/spidev0.0";
static const char *spiDev1 = "/dev/spidev0.1";
static const uint8_t spiBPW = 8;
static const uint16_t spiDelay = 0;

static uint32_t spi_speeds[2];
static int spi_fds[2];

int spi_get_fd(int channel) {
    return spi_fds[channel & 1];
}

int spi_write_byte(int channel, uint8_t data) {
    uint8_t buffer[1] = {data};
    return spi_write(channel, buffer, 1);
}

int spi_write(int channel, uint8_t *buffer, unsigned int len) {
    channel &= 1;

    // Half duplex write
    int ret;

    uint8_t rBuf[len];

    struct spi_ioc_transfer tr = {};
    tr.tx_buf = (unsigned long) buffer;
    tr.rx_buf = (unsigned long) rBuf;
    tr.len = len;
    tr.delay_usecs = spiDelay;
    tr.speed_hz = spi_speeds[channel];
    tr.bits_per_word = spiBPW;

    ret = ioctl(spi_fds[channel], SPI_IOC_MESSAGE(1), &tr);

    if (ret < 1) {
        cerr << "Unable to send spi message for write operation: " << errstr(errno) << endl;
    }

    return ret;
}

int spi_read(int channel, uint8_t *buffer, unsigned int len) {
    channel &= 1;

    // Half duplex read
    int ret;
    struct spi_ioc_transfer tr = {};
    tr.tx_buf = 0;
    tr.rx_buf = (unsigned long) buffer;
    tr.len = len;
    tr.delay_usecs = spiDelay;
    tr.speed_hz = spi_speeds[channel];
    tr.bits_per_word = spiBPW;

    ret = ioctl(spi_fds[channel], SPI_IOC_MESSAGE(1), &tr);

    if (ret < 1) {
        cerr << "Unable to send spi message for read operation: " << errstr(errno) << endl;
    }

    return ret;
}

int spi_transfer(int channel, unsigned char *buffer, int len) {
    struct spi_ioc_transfer spi{};

    channel &= 1;

    memset(&spi, 0, sizeof(spi));

    spi.tx_buf = (unsigned long) buffer;
    spi.rx_buf = (unsigned long) buffer;
    spi.len = len;
    spi.delay_usecs = spiDelay;
    spi.speed_hz = spi_speeds[channel];
    spi.bits_per_word = spiBPW;

    return ioctl(spi_fds[channel], SPI_IOC_MESSAGE(1), &spi);
}

int spi_setup(int channel, int speed, int mode) {
    int fd;

    mode &= 3;    // Mode is 0, 1, 2 or 3
    channel &= 1;    // Channel is 0 or 1

    if ((fd = open(channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0) {
        cerr << "Unable to open SPI device:" << errstr(errno) << endl;
        return -1;
    }

    spi_speeds[channel] = static_cast<uint32_t>(speed);
    spi_fds[channel] = fd;

    // Set SPI parameters.
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        cerr << "SPI Mode Change failure: " << errstr(errno) << endl;
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0) {
        cerr << "SPI BPW Change failure: " << errstr(errno) << endl;
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        cerr << "SPI Speed Change failure: " << errstr(errno) << endl;
        return -1;
    }

    return fd;
}