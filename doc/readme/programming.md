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
- If you wish to apply one or more of the device tree overlays from the
  [dts][dts] directory, enter the following command to build the firmware:
  ```
  west build -b actinius_icarus_ns -p auto -- -DDTC_OVERLAY_FILE=<paths>
  ```
  where `<paths>` is a space- or semicolon-separated list of paths to the
  desired overlay files. For example, the following command applies the
  [dts/console\_uart1.overlay][console_uart1.overlay] and
  [dts/sim\_external.overlay][sim_external.overlay] device tree
  overlays while building:
  ```
  west build -b actinius_icarus_ns -p auto \
    -- -DDTC_OVERLAY_FILE="dts/console_uart1.overlay dts/sim_external.overlay"
  ```

The effect of applying the available device tree overlays is summarised below:

| **Overlay**                                         | **Effect**            |
| --------------------------------------------------- | --------------------- |
| [dts/console\_uart0.overlay][console_uart0.overlay] | Console on UART0      |
| [dts/console\_uart1.overlay][console_uart1.overlay] | Console on UART1      |
| [dts/sim\_internal.overlay][sim_internal.overlay]   | Use internal eSIM     |
| [dts/sim\_external.overlay][sim_external.overlay]   | Use external nano SIM |

To flash the firmware onto the device, connect the J-Link probe as described in
[doc/readme/requirements.md][requirements.md] and enter the following command:
```
west flash --erase
```

Once the flashing process is complete, the application will automatically start.

[dts]:                    ../../dts
[console_uart0.overlay]:  ../../dts/console_uart0.overlay
[console_uart1.overlay]:  ../../dts/console_uart1.overlay
[sim_internal.overlay]:   ../../dts/sim_internal.overlay
[sim_external.overlay]:   ../../dts/sim_external.overlay
[requirements.md]:        requirements.md
