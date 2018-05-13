/**
 * Open and configure the given SPI device using settings for the lis3dh.
 */
int open_spi_device_as_lis3dh(const char *device_name, uint32_t speed_hz);

/**
 * Close the SPI device.
 */
void close_spi_device(int fd);
