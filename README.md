# nRF9160 Logger

The *nRF9160 Logger* is a configurable logging device based on the
[Actinius Icarus v2](https://www.actinius.com/icarus) development board, which
features the [nRF9160](https://www.nordicsemi.com/products/nrf9160) SiP. This
logger can be configured to regularly upload dummy data, LTE network
information, or GNSS fixes to an online database. For uploading data, the device
accesses the internet over the LTE-M or NB-IoT network.

Detailed information about this project is provided in the following files. It
is recommended to read them in the order in which they are listed here:

- [doc/readme/description.md](doc/readme/description.md): Provides a brief
  overview of the logging process implemented on the device. Essentially
  describes what the device does.
- [doc/readme/requirements.md](doc/readme/requirements.md): Lists out the
  prerequisites in terms of both, hardware as well as software, that are needed
  to assemble and program the device.
- [doc/readme/configuration.md](doc/readme/configuration.md): Contains
  instructions on how to configure the device firmware to suit your application.
- [doc/readme/programming.md](doc/readme/programming.md): Contains instructions
  for building the firmware from source code and flashing it onto the device.
- [doc/readme/usage.md](doc/readme/usage.md): Describes how to use the device
  after the firmware has been flashed.

The main purpose of this project is to experimentally study the power
consumption involved in GNSS tracking and data logging using cellular IoT
networks. The software is, therefore, designed according to this goal, but it
should be easy to adapt it for other purposes.
