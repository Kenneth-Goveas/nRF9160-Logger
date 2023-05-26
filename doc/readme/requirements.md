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

- An Actinius Icarus v2 development board.
- An LTE-M/NB-IoT antenna.
- A GNSS antenna.
- A micro-USB cable.

To assemble the device, simply attach both antennae to their respective
connectors on the development board.

## Software installation

The following software is required to build and flash the device firmware:

- The nRF Connect SDK.
- The MCUmgr command line tool.

For instructions on installing the
[nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html),
see
[Installing automatically](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started/assistant.html).
In
[Set up the preferred building method](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started/assistant.html#set-up-the-preferred-building-method),
you will have the option to choose either VS Code or the command line as your
development environment. The command line is recommended, as the instructions
given further in this project documentation are provided for the command line.

To install the
[MCUmgr](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/services/device_mgmt/mcumgr.html)
command line tool, see
[Command line tool](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/services/device_mgmt/mcumgr.html#command-line-tool).
You may have to first install [Go](https://go.dev/doc/install) before being able
to install MCUmgr.

## Database setup

The device, if appropriately configured, should be able to upload to any
database that offers a secure REST API. However, [restdb.io](https://restdb.io)
is recommended as this has been tested.

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
must and also note down the full access API key for the database
(see [API keys and CORS Ajax calls](https://restdb.io/docs/apikeys-and-cors)).
