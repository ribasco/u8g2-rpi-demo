//
// Created by raffy on 6/27/18.
//

#ifndef U8G2_INTEGRATION_SPI_H
#define U8G2_INTEGRATION_SPI_H

#define SPI_DEV_MODE_0 0
#define SPI_DEV_MODE_1 1
#define SPI_DEV_MODE_2 2
#define SPI_DEV_MODE_3 3

/**
 * Get the current file descriptor for the specified SPI Channel
 * @param channel The spi channel (0 or 1)
 * @return The file descriptor
 */
int spi_get_fd(int channel);

/**
 * Writes a single byte to the SPI bus. This is a half-duplex operation.
 *
 * @param channel
 * @param data
 * @return
 */
int spi_write(int channel, uint8_t data);

/**
 * Half-Duplex write operation. Chip-select is deactivated in this operation and you need to manually set/unset it yourself.
 *
 * @param channel
 * @param txBuffer
 * @param len
 * @return Returns 0 for successful operation otherwise -1 if an error occurs
 *
 * @see <a href="https://www.kernel.org/doc/Documentation/spi/spidev">SPI kernel documentation</a>
 */
int spi_write(int channel, uint8_t *txBuffer, unsigned int len);

/**
 * Full-Duplex write operation. This method have separate buffers for send/receive, both buffers should be of the same length.
 *
 * @param channel The SPI channel (0 or 1)
 * @param txBuffer The transmit buffer
 * @param rxBuffer The receive buffer
 * @param len The length of the two buffers
 * @return Returns 0 for successful operation otherwise -1 if an error occurs
 */
int spi_write(int channel, uint8_t *txBuffer, uint8_t *rxBuffer, unsigned int len);

/**
 * Half-Duplex read operation. Chip-select is deactivated in this operation and you need to manually set/unset it yourself.
 *
 * @param channel
 * @param buffer
 * @param len
 *
 * @return Returns 0 for successfull operation otherwise negative if an error occurs
 *
 * @see <a href="https://www.kernel.org/doc/Documentation/spi/spidev">SPI kernel documentation</a>
 */
int spi_read(int channel, uint8_t *buffer, unsigned int len);

/**
 * Full Duplex read/write operation. Received data overrides
 *
 * @param channel
 * @param buffer
 * @param len
 * @return
 */
int spi_transfer(int channel, uint8_t *buffer, int len);

/**
 * Initialize SPI Device
 *
 * @param channel
 * @param speed
 * @param mode
 * @return
 */
int spi_setup(int channel, int speed, int mode = 0);

#endif //U8G2_INTEGRATION_SPI_H
