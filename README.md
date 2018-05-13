# lis3dh-sandbox

C code to interface with a lis3dh accelerometer over SPI using the Linux spidev driver. It has been tested on a Raspberry Pi.

This code was written as a starting point for a specific project and does not expose many of the lis3dh options. Any PRs are welcome, however.

To build and run:

```
make && ./main
```

# License

This program was derived from [spidev_test.c](https://github.com/torvalds/linux/blob/ccda3c4b77777f66aeb3c531352bb40d59501c59/tools/spi/spidev_test.c). 
