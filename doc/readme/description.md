# The logging process

The nRF9160 Logger operates in a repetitive manner. In each cycle, the device
first enters sleep mode for a fixed time period, during which it consumes low
power. Upon waking up, it attempts to obtain and upload the relevant data to the
database server. The exact process depends on what data the device is configured
to upload - dummy data, LTE network information, or GNSS fixes - as described
below.

## Dummy logging

If configured to upload dummy data, the device first constructs a dummy data
frame that occupies 256 bytes. It then attempts to establish an LTE-M/NB-IoT
connection and upload this data frame to the server. After this, the device
disconnects from the network and returns to sleep mode.

## LTE logging

If configured to upload LTE network information, the device first connects to
the LTE-M/NB-IoT network. After this, it waits for notifications containing
network-related data, such as cell information, power saving parameters, etc.
Every time a notification is received, a corresponding data frame is constructed
and uploaded to the server, following which the device starts waiting for the
next notification. For every such wait, if a timeout expires before a
notification is received, the device disconnects from the network and returns to
sleep mode.

## GNSS logging

If configured to upload GNSS fixes, the device first activates its GNSS
interface and waits until either a fix is obtained or a timeout expires. The
GNSS interface is then deactivated. If no fix was obtained, the device
immediately returns to sleep mode. Otherwise, it connects to the LTE-M/NB-IoT
network and uploads the GNSS data frame to the server. After this, it
disconnects from the network and returns to sleep mode.
