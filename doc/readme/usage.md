# Device usage

After the firmware has been successfully flashed onto the device, the logging
application will automatically start on the next reboot. The uploaded data
frames will be visible on the database backend. If you left UART logging
enabled, the device will log messages onto the console. If you didn't use a
device tree overlay, logging is performed over the default UART0 port, which is
accessed through the micro-USB connector. If you used an overlay that sets the
consoele to UART1, the connection is accessible through the RX and TX pins on
the device. In both cases, the UART baud rate is 115200. You can use any serial
terminal app (such as
[Serial Terminal](https://github.com/Kenneth-Goveas/Serial-Terminal) for Linux)
to read out these messages. If you turned off UART to optimize the power
consumption, no messages will be received.
