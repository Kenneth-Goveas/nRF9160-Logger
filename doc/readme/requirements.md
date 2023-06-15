# Prerequisites

The following requirements must be satisfied in order to be able to assemble,
program, and use an nRF9160 Logger.

## System requirements

All the steps described in this project documentation were tested and found to
work on a Linux machine (in particular, a system running
[Pop!\_OS 22.04][pop-os-22.04]) but are expected to work on other platforms too.

## Hardware assembly

The following parts are needed to assemble and program the device:

- An [Actinius Icarus v2][actinius-icarus-v2] development board.
- An [LTE-M/NB-IoT antenna][ltem-nbiot-antenna].
- A [GNSS antenna][gnss-antenna].
- A [J-Link probe][jlink-probe].
- A [TC2030-IDC-NL][tc2030-idc-nl] programming cable.
- A [ARM20-CTX][arm20-ctx] cable adapter.
- Two micro-USB cables.

To assemble the device, simply attach both antennae to their respective
connectors on the development board and power it through the USB connector. To
program the device, connect the J-Link probe to your computer with another USB
cable and to the development board with the TC2030-IDC-NL cable and ARM20-CTX
adapter.

## Software installation

The [nRF Connect SDK v2.3.0][nrf-connect-sdk-v2.3.0] is required to build and
flash the device firmware (while other SDK versions are also expected to work,
this has not been tested). For installation instructions, see
[Installing automatically][installing-automatically]. In
[Set up the preferred building method][set-up-the-preferred-build-method], you
will have the option to choose either VS Code or the command line as your
development environment. The command line is recommended, as the instructions
given further in this project documentation are provided for the command line.

## Database setup

The device, if appropriately configured, should be able to upload to any
database with a compatible REST API. However, [restdb.io][restdb.io] is
recommended as this has been tested and should work out-of-the-box.

To set up the database, you must create an account and follow the instructions
in [Quick start][quick-start] to create the following three collections:

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
(see [API keys and CORS Ajax calls][api-keys-and-cors-ajax-calls]).

[pop-os-22.04]:                       https://pop.system76.com
[actinius-icarus-v2]:                 https://www.actinius.com/icarus
[ltem-nbiot-antenna]:                 https://www.te.com/usa-en/product-ANT-B8-PW-QW-UFL.html
[gnss-antenna]:                       https://www.te.com/usa-en/product-ANT-GNCP-C25L12100.html
[jlink-probe]:                        https://shop.segger.com/debug-trace-probes/debug-probes/j-link/j-link-base-compact
[tc2030-idc-nl]:                      https://www.tag-connect.com/product/tc2030-idc-nl
[arm20-ctx]:                          https://www.tag-connect.com/product/arm20-ctx-20-pin-to-tc2030-idc-adapter-for-cortex
[nrf-connect-sdk-v2.3.0]:             https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html
[installing-automatically]:           https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/assistant.html
[set-up-the-preferred-build-method]:  https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started/assistant.html#set-up-the-preferred-building-method
[restdb.io]:                          https://restdb.io
[quick-start]:                        https://restdb.io/docs/quick-start
[api-keys-and-cors-ajax-calls]:       https://restdb.io/docs/apikeys-and-cors
