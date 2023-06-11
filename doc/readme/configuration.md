# Configuring the application

The application can be configured to suit your requirements by assigning the
parameters in the [prj.conf](../../prj.conf) configuration file. A bare minimum
initial setup is required for the application to work at all. Beyond this, it is
also possible to optimize it for low power usage, enable/disable different
network modes, adjust the sleep duration and timeouts, and fine tune many other
features.

## Bare minimum

While most parameters may be left at their default values, the following
parameters must necessarily be reconfigured for the application to work:

| **Parameter**                   | **Description**                 |
| ------------------------------- | -----------------------------   |
| `CONFIG_MAIN_DATA_TYPE_*`       | Type of data frames to upload   |
| `CONFIG_MAIN_DUMMY_UPLOAD_URL`  | URL for dummy data upload       |
| `CONFIG_MAIN_LTE_UPLOAD_URL`    | URL for LTE data upload         |
| `CONFIG_MAIN_GNSS_UPLOAD_URL`   | URL for GNSS data upload        |
| `CONFIG_REST_HOST_NAME`         | Host name of database server    |
| `CONFIG_REST_API_KEY`           | Full access API key of database |
| `CONFIG_REST_SEC_TAG`           | TLS security tag                |

The parameter `CONFIG_MAIN_DATA_TYPE_*` selects the data type to be uploaded.
There are three choices for this parameter - `CONFIG_MAIN_DATA_TYPE_DUMMY`,
`CONFIG_MAIN_DATA_TYPE_LTE`, and `CONFIG_MAIN_DATA_TYPE_GNSS` - of which exactly
one must be selected by setting the appropriate parameter to `y`. Thus, to
upload GNSS fixes, you must set `CONFIG_MAIN_DATA_TYPE_GNSS=y`. The default
configuration already sets `CONFIG_MAIN_DATA_TYPE_DUMMY=y` so you need to simply
edit this line to enable whichever parameter is appropriate.

The parameters `CONFIG_MAIN_*_UPLOAD_URL` must be assigned the URLs of the
collections you created while setting up the database server, and
`CONFIG_REST_HOST_NAME` must be assigned the host name of the server. The URLs
must not contain the server host name. Thus, if the endpoint for dummy data
upload is `https://database-name.restdb.io/rest/dummy-collection`, then
`CONFIG_MAIN_DUMMY_UPLOAD_URL` should be assigned the value
`"/rest/dummy-collection"` and `CONFIG_REST_HOST_NAME` should be assigned the
value `"database-name.restdb.io"`.

The parameter `CONFIG_REST_API_KEY` should be assigned the full access API key
for the database, which you must have noted down while setting up the database
server.

The security tag `CONFIG_REST_SEC_TAG` must correspond to an installed CA
certificate on the nRF9160 modem. A certificate may be installed by simply
flashing the [nRF9160 Certificate Installer](https://github.com/Kenneth-Goveas/nRF9160-Certificate-Installer)
firmware onto the device. The value assigned to `CONFIG_REST_SEC_TAG` must be
identical to the security tag used while installing the certificate.
Certificate installation is persistent and therefore, needs to be done only
once. Since TLS peer verification is disabled, it is not necessary to install
the actual certificate for the database server. However, a dummy certificate is
still needed. Otherwise, the networking libraries default to unencrypted
connections.

With this basic configuration, the application may already be built and flashed
onto the device. However, if you wish to make further tweaks to it, then
continue reading.

## Power optimization

In the default configuration, the UART is enabled and used for message logging.
However, this incurs a significant rise in power consumption in sleep mode, and
must be turned off if you want to optimize the application for low power usage.
To do so, the following parameters must be reconfigured:

| **Parameter**           | **Description**           |
| ----------------------- | ------------------------- |
| `CONFIG_SERIAL`         | Enable serial driver      |
| `CONFIG_STDOUT_CONSOLE` | Direct stdout to console  |
| `CONFIG_LOG`            | Enable message logging    |

To disable the UART entirely, all these parameters must be set to `n`.

## Network setup

The modem supports LTE-M and NB-IoT networks as well as GNSS reception.
Depending on the application's requirements, each of these can be individually
enabled. The LTE power saving features, PSM and eDRX, can also be configured.
The parameters of interest are the following:

| **Parameter**                     | **Description**         |
| --------------------------------- | ----------------------- |
| `CONFIG_LTE_NETWORK_MODE_*`       | Enabled network mode    |
| `CONFIG_LTE_USE_PSM`              | Enable PSM              |
| `CONFIG_LTE_USE_EDRX`             | Enable eDRX             |
| `CONFIG_LTE_PSM_REQ_RPTAU`        | PSM periodic TAU timer  |
| `CONFIG_LTE_PSM_REQ_RAT`          | PSM active timer        |
| `CONFIG_LTE_EDRX_REQ_VALUE_LTE_M` | eDRX timer for LTE-M    |
| `CONFIG_LTE_EDRX_REQ_VALUE_NBIOT` | eDRX timer for NB-IoT   |

The parameter `CONFIG_LTE_NETWORK_MODE_*` selects the network modes that must be
enabled in the application. There are six choices for this parameter -
`CONFIG_LTE_NETWORK_MODE_LTE_M`, `CONFIG_LTE_NETWORK_MODE_NBIOT`,
`CONFIG_LTE_NETWORK_MODE_LTE_M_NBIOT`, `CONFIG_LTE_NETWORK_MODE_LTE_M_GPS`,
`CONFIG_LTE_NETWORK_MODE_NBIOT_GPS`, and
`CONFIG_LTE_NETWORK_MODE_LTE_M_NBIOT_GPS` - of which exactly one must be
selected by setting the appropriate parameter to `y`. Thus, to enable LTE-M and
GNSS but not NB-IoT, you must set `CONFIG_LTE_NETWORK_MODE_LTE_M_GPS=y`. The
default configuration already sets `CONFIG_LTE_NETWORK_MODE_LTE_M_NBIOT_GPS=y`
so you need to simply edit this line to enable whichever parameter is
appropriate.

The power saving features, PSM and eDRX, may be separately enabled or disabled
by setting `CONFIG_LTE_USE_PSM` and `CONFIG_LTE_USE_EDRX` respectively to `y` or
`n`. The timers for PSM and eDRX must be assigned as described in the
documentation of the AT commands, [AT+CPSMS](https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/nw_service/cpsms_set.html)
and [AT+CEDRXS](https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/nw_service/cedrxs_set.html).

## Sleep duration and timeouts

The sleep duration and all the timeouts used by the application can be tuned by
configuring the following parameters:

| **Parameter**               | **Description**                     |
| --------------------------- | ----------------------------------- |
| `CONFIG_MAIN_SLEEP_TIME`    | Sleep duration in seconds           |
| `CONFIG_LTE_CONN_TIMEOUT`   | LTE connection timeout in seconds   |
| `CONFIG_LTE_DATA_TIMEOUT`   | LTE data update timeout in seconds  |
| `CONFIG_GNSS_DATA_TIMEOUT`  | GNSS data update timeout in seconds |
| `CONFIG_REST_REQ_TIMEOUT`   | REST request timeout in seconds     |

The `CONFIG_MAIN_SLEEP_TIME` parameter specifies the time period for sleep mode.

The `CONFIG_LTE_CONN_TIMEOUT` parameter specifies the timeout for establishing
an LTE-M/NB-IoT connection. If this timeout expires before the device manages to
connect to the network, the attempt is considered failed and the LTE interface
is deactivated. Any data that was to be uploaded will be lost.

The `CONFIG_LTE_DATA_TIMEOUT` parameter specifies the timeout for receiving
notifications containing LTE network information. This timeout is used in LTE
data logging to decide when it is time to return to sleep mode.

The `CONFIG_GNSS_DATA_TIMEOUT` parameter specifies the timeout for receiving a
GNSS fix. This timeout is used in GNSS logging to decide if a GNSS fix attempt
has failed and the device should return to sleep mode without uploading data.

The `CONFIG_REST_REQ_TIMEOUT` parameter specifies the timeout for HTTP requests
to the database server.

## Other parameters

Configuration parameters not described above may also be reconfigured to finely
adjust the application to suit your requirements. The application-specific
parameters are described in [Kconfig](../../Kconfig). Other configuration
parameters are defined by the nRF Connect SDK and are documented in the
[Kconfig reference](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/kconfig/index.html).
