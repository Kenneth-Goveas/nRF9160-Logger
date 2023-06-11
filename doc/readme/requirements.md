# Prerequisites

The following requirements must be satisfied in order to be able to assemble,
program, and use an nRF9160 Logger.

## System requirements

All the steps described in this project documentation were tested and found to
work on a Linux machine (specifically, a system running
[Pop!\_OS 22.04](https://pop.system76.com)) but are expected to work on other
platforms too.

## Hardware assembly

The following parts are needed to assemble and program the device:

- An [Actinius Icarus v2](https://www.actinius.com/icarus) development board.
- An [LTE-M/NB-IoT antenna](https://www.te.com/usa-en/product-ANT-B8-PW-QW-UFL.html).
- A [GNSS antenna](https://www.te.com/usa-en/product-ANT-GNCP-C25L12100.html).
- A [J-Link probe](https://shop.segger.com/debug-trace-probes/debug-probes/j-link/j-link-base-compact).
- A [TC2030-IDC-NL](https://www.tag-connect.com/product/tc2030-idc-nl)
  programming cable.
- A [ARM20-CTX](https://www.tag-connect.com/product/arm20-ctx-20-pin-to-tc2030-idc-adapter-for-cortex)
  cable adapter.
- Two micro-USB cables.

To assemble the device, simply attach both antennae to their respective
connectors on the development board and power it through the USB connector. To
program the device, connect the J-Link probe to your computer with another USB
cable and to the development board with the TC2030-IDC-NL cable and ARM20-CTX
adapter.

## Software installation

The [nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html)
version 2.3.0 is required to build and flash the device firmware (while other
SDK versions are also expected to work, this has not been tested). For
installation instructions, see [Installing automatically](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/assistant.html).
In [Set up the preferred building method](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/assistant.html#set-up-the-preferred-building-method),
you will have the option to choose either VS Code or the command line as your
development environment. The command line is recommended, as the instructions
given further in this project documentation are provided for the command line.

## Database setup

The device, if appropriately configured, should be able to upload to any
database that offers a secure REST API. However, [restdb.io](https://restdb.io)
is recommended as this has been tested and should work out-of-the-box.

To set up the database, you must create an account and follow the instructions
in [Quick start](https://restdb.io/docs/quick-start) to create the following
three collections:

- Collection for dummy data, with the following four fields:

  | **Field name** | **Data type** |
  | -------------- | ------------- |
  | field1         | text          |
  | field2         | text          |
  | field3         | text          |
  | field4         | text          |

- Collection for LTE data, with the following four fields:

  | **Field name** | **Data type** |
  | -------------- | ------------- |
  | mode           | json          |
  | cell           | json          |
  | psm            | json          |
  | edrx           | json          |

- Collection for GNSS data, with the following three fields:

  | **Field name** | **Data type** |
  | -------------- | ------------- |
  | location       | json          |
  | date           | json          |
  | time           | json          |

After setting up the database, you must find and note down the endpoint URLs for
each of the above three collections you created (these are visible in the
collection settings in developer mode and not in your browser's search bar). You
must also note down the full access API key for the database
(see [API keys and CORS Ajax calls](https://restdb.io/docs/apikeys-and-cors)).
