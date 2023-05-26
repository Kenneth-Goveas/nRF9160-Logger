# Building and flashing the firmware

To build the firmware, carry out the following steps:

- Open a Bash terminal through the Toolchain Manager app you must have obtained
  while installing the nRF Connect SDK (it is necessary to use the Toolchain
  Manager to open the terminal as this sets up the development environment for
  the nRF Connect SDK).
- Navigate to the directory where you cloned this repository.
- If you wish to use the default device tree, build the firmware with the
  following command:
  ```
  west build -b actinius_icarus_ns -p auto
  ```
- If you wish to apply one of the device tree overlays from the [dts](../../dts)
  directory, enter the following command to build the firmware:
  ```
  west build -b actinius_icarus_ns -p auto -- -DDTC_OVERLAY_FILE=<path>
  ```
  where `<path>` is the path to the selected overlay file. For example, the
  following command applies the
  [dts/console\_uart1.overlay](../../dts/console_uart1.overlay) device tree
  overlay while building:
  ```
  west build -b actinius_icarus_ns -p auto \
    -- -DDTC_OVERLAY_FILE=dts/console_uart1.overlay
  ```

The effect of applying the available device tree overlays is summarised below:

| **Overlay**                                                   | **Effect**                                    |
| ------------------------------------------------------------- | --------------------------------------------- |
| [dts/console\_uart0.overlay](../../dts/console_uart0.overlay) | Console on UART0, connected to micro-USB      |
| [dts/console\_uart1.overlay](../../dts/console_uart1.overlay) | Console on UART1, connected to RX and TX pins |

By selecting the console, messages logged can be directed to the UART port of
your choice.

To flash the firmware onto the device, connect it with a micro-USB cable and
follow the instructions below:

- Press and hold the reset button, and then press and hold the user button.
- Release the reset button, followed by the user button.
- Flash the firmware with the following command:
  ```
  mcumgr image upload build/zephyr/app_update.bin \
    --conntype=serial --connstring="dev=<port>,baud=115200"
  ```
  where `<port>` is the serial port connected to the device. On Linux, this
  would be something like `/dev/ttyUSB<n>` or `/dev/ttyACM<n>`.

Once the flashing process is complete, the application will automatically start
on the next device reboot.
