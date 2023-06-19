#include <stdbool.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <modem/lte_lc.h>
#include <nrf_modem_gnss.h>

#include "data.h"

// Register module for logging.
LOG_MODULE_REGISTER(gnss, CONFIG_GNSS_LOG_LEVEL);

// Data available flag. Value indicates whether or not data is available.
// Semaphore is released whenever value is set to true, to signal threads
// waiting for data to become available. Mutex protects against concurrent
// access, as this is a shared resource.
static K_SEM_DEFINE(_gnss_data_avail_sem, 0, 1);
static K_MUTEX_DEFINE(_gnss_data_avail_mutex);
static bool _gnss_data_avail_flag = false;

// Internally stored data frame. Mutex protects against concurrent access, as
// this is a shared resource.
static K_MUTEX_DEFINE(_gnss_data_frame_mutex);
static gnss_data_frame_t _gnss_data_frame = {
    .loc = {
        .valid = false,
        .lat = {
            .dir = "",
            .deg = 0,
            .min = 0,
            .sec = 0,
            .msec = 0
        },
        .lon = {
            .dir = "",
            .deg = 0,
            .min = 0,
            .sec = 0,
            .msec = 0
        }
    },
    .date = {
        .valid = false,
        .year = 0,
        .mon = 0,
        .day = 0
    },
    .time = {
        .valid = false,
        .hour = 0,
        .min = 0,
        .sec = 0,
        .msec = 0
    }
};

static void _gnss_handler (int evt) {
    int status;                                 // Return status for API calls.
    struct nrf_modem_gnss_pvt_data_frame pvt;   // PVT solution.

    // Check event type and handle event accordingly.
    switch (evt) {
        case NRF_MODEM_GNSS_EVT_PVT:
            /*
             * Fix obtained. Parse fix data and if fix is valid, update data
             * frame and indicate data availability. If an error occurs in this
             * process, report failure and exit.
             */

            // Parse fix data.

            status = nrf_modem_gnss_read(
                &pvt, sizeof(pvt), NRF_MODEM_GNSS_DATA_PVT
            );

            if (status < 0) {
                // On error, report failure and exit.
                LOG_ERR(
                    "Failed to parse GNSS fix (%s)",
                    strerror(-status)
                );
            } else if (pvt.flags & NRF_MODEM_GNSS_PVT_FLAG_FIX_VALID) {
                /*
                 * On valid fix, update data frame, and indicate data
                 * availability.
                 */

                // Block other threads from accessing shared resources.
                k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
                k_mutex_lock(&_gnss_data_frame_mutex, K_FOREVER);

                // Indicate data availability by setting flag value to true and
                // releasing semaphore.
                k_sem_give(&_gnss_data_avail_sem);
                _gnss_data_avail_flag = true;

                // Update data frame.

                _gnss_data_frame.loc.valid = true;

                if (pvt.latitude < 0) {
                    pvt.latitude = -pvt.latitude;
                    _gnss_data_frame.loc.lat.dir = "S";
                } else {
                    _gnss_data_frame.loc.lat.dir = "N";
                }

                _gnss_data_frame.loc.lat.deg = (int)(
                    pvt.latitude
                );
                _gnss_data_frame.loc.lat.min = (int)(
                    60 * pvt.latitude
                    - 60 * _gnss_data_frame.loc.lat.deg
                );
                _gnss_data_frame.loc.lat.sec = (int)(
                    3600 * pvt.latitude
                    - 3600 * _gnss_data_frame.loc.lat.deg
                    - 60 * _gnss_data_frame.loc.lat.min
                );
                _gnss_data_frame.loc.lat.msec = (int)(
                    3600000 * pvt.latitude
                    - 3600000 * _gnss_data_frame.loc.lat.deg
                    - 60000 * _gnss_data_frame.loc.lat.min
                    - 1000 * _gnss_data_frame.loc.lat.sec
                );

                if (pvt.longitude < 0) {
                    pvt.longitude = -pvt.longitude;
                    _gnss_data_frame.loc.lon.dir = "W";
                } else {
                    _gnss_data_frame.loc.lon.dir = "E";
                }

                _gnss_data_frame.loc.lon.deg = (int)(
                    pvt.longitude
                );
                _gnss_data_frame.loc.lon.min = (int)(
                    60 * pvt.longitude
                    - 60 * _gnss_data_frame.loc.lon.deg
                );
                _gnss_data_frame.loc.lon.sec = (int)(
                    3600 * pvt.longitude
                    - 3600 * _gnss_data_frame.loc.lon.deg
                    - 60 * _gnss_data_frame.loc.lon.min
                );
                _gnss_data_frame.loc.lon.msec = (int)(
                    3600000 * pvt.longitude
                    - 3600000 * _gnss_data_frame.loc.lon.deg
                    - 60000 * _gnss_data_frame.loc.lon.min
                    - 1000 * _gnss_data_frame.loc.lon.sec
                );

                _gnss_data_frame.date.valid = true;
                _gnss_data_frame.date.year = pvt.datetime.year;
                _gnss_data_frame.date.mon = pvt.datetime.month;
                _gnss_data_frame.date.day = pvt.datetime.day;

                _gnss_data_frame.time.valid = true;
                _gnss_data_frame.time.hour = pvt.datetime.hour;
                _gnss_data_frame.time.min = pvt.datetime.minute;
                _gnss_data_frame.time.sec = pvt.datetime.seconds;
                _gnss_data_frame.time.msec = pvt.datetime.ms;

                LOG_INF(
                    "Obtained GNSS fix: "
                    "%d°%d'%d.%03d\"%s %d°%d'%d.%03d\"%s "
                    "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                    _gnss_data_frame.loc.lat.deg, _gnss_data_frame.loc.lat.min,
                    _gnss_data_frame.loc.lat.sec, _gnss_data_frame.loc.lat.msec,
                    _gnss_data_frame.loc.lat.dir,
                    _gnss_data_frame.loc.lon.deg, _gnss_data_frame.loc.lon.min,
                    _gnss_data_frame.loc.lon.sec, _gnss_data_frame.loc.lon.msec,
                    _gnss_data_frame.loc.lon.dir,
                    _gnss_data_frame.date.year, _gnss_data_frame.date.mon,
                    _gnss_data_frame.date.day,
                    _gnss_data_frame.time.hour, _gnss_data_frame.time.min,
                    _gnss_data_frame.time.sec, _gnss_data_frame.time.msec
                );

                // Allow other threads to access shared resources.
                k_mutex_unlock(&_gnss_data_avail_mutex);
                k_mutex_unlock(&_gnss_data_frame_mutex);
            }
            break;
        default:
            break;
    }
}

int gnss_init (void) {
    int status; // Return status for API calls.

    /*
     * Initialize modem. If an error occurs in this process, exit with failure.
     */

    LOG_INF("Initializing modem");

    // Initialize modem.
    status = lte_lc_init();
    if (status < 0) {
        // On error, exit with failure.
        LOG_ERR(
            "Failed to initialize modem (%s)",
            strerror(-status)
        );
        return -1;
    }

    /*
     * Initialize GNSS system. The correct sequence is to activate the GNSS
     * interface, configure the GNSS to operate in single fix mode by setting
     * the periodic fix interval as well as the fix retry period to zero, and
     * finally register the GNSS event handler. If an error occurs anywhere in
     * this process, deinitialize the modem and exit with failure.
     */

    LOG_INF("Initializing GNSS");

    // Activate GNSS but not LTE.
    status = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_ACTIVATE_GNSS);
    if (status < 0) {
        // On error, deinitialize modem and exit with failure.
        LOG_ERR(
            "Failed to set modem to GNSS active mode (%s)",
            strerror(-status)
        );
        LOG_WRN("Deinitializing modem");
        status = lte_lc_deinit();
        if (status < 0) {
            LOG_ERR(
                "Failed to deinitialize modem (%s)",
                strerror(-status)
            );
        }
        return -1;
    }

    // Set periodic fix interval to zero.
    status = nrf_modem_gnss_fix_interval_set(0);
    if (status < 0) {
        // On error, deinitialize modem and exit with failure.
        LOG_ERR(
            "Failed to set GNSS fix interval (%s)",
            strerror(-status)
        );
        LOG_WRN("Deinitializing modem");
        status = lte_lc_deinit();
        if (status < 0) {
            LOG_ERR(
                "Failed to deinitialize modem (%s)",
                strerror(-status)
            );
        }
        return -1;
    }

    // Set fix retry interval to zero.
    status = nrf_modem_gnss_fix_retry_set(0);
    if (status < 0) {
        // On error, deinitialize modem and exit with failure.
        LOG_ERR(
            "Failed to set GNSS fix retry period (%s)",
            strerror(-status)
        );
        LOG_WRN("Deinitializing modem");
        status = lte_lc_deinit();
        if (status < 0) {
            LOG_ERR(
                "Failed to deinitialize modem (%s)",
                strerror(-status)
            );
        }
        return -1;
    }

    // Register GNSS event handler.
    nrf_modem_gnss_event_handler_set(_gnss_handler);

    /*
     *  Start GNSS reception. If an error occurs in this process, deinitialize
     *  the modem and exit with failure.
     */

    LOG_INF("Starting GNSS");

    // Start GNSS reception.
    status = nrf_modem_gnss_start();
    if (status < 0) {
        // On error, deinitialize modem and exit with failure.
        LOG_ERR(
            "Failed to start GNSS (%s)",
            strerror(-status)
        );
        LOG_WRN("Deinitializing modem");
        status = lte_lc_deinit();
        if (status < 0) {
            LOG_ERR(
                "Failed to deinitialize modem (%s)",
                strerror(-status)
            );
        }
        return -1;
    }

    return 0;
}

void gnss_deinit (void) {
    int status; // Return status for API calls.

    /*
     * Stop GNSS reception. If an error occurs in this process, report failure
     * but proceed anyway to deinitialize the modem.
     */

    LOG_WRN("Stopping GNSS");

    // Stop GNSS reception.
    status = nrf_modem_gnss_stop();
    if (status < 0) {
        // On error, report failure.
        LOG_ERR(
            "Failed to stop GNSS (%s)",
            strerror(-status)
        );
    }

    /*
     * Deinitialize modem. If an error occurs in this process, report failure.
     */

    LOG_WRN("Deinitializing modem");

    // Deinitialize modem.
    status = lte_lc_deinit();
    if (status < 0) {
        // On error, report failure.
        LOG_ERR(
            "Failed to deinitialize modem (%s)",
            strerror(-status)
        );
    }
}

bool gnss_data_avail (void) {
    bool flag;  // Non-shared copy of flag value.

    // Safely copy flag value to non-shared variable.
    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    flag = _gnss_data_avail_flag;
    k_mutex_unlock(&_gnss_data_avail_mutex);

    return flag;
}

int gnss_wait_data_avail (void) {
    int status; // Return status for API calls.
    bool flag;  // Non-shared copy of flag value.

    // Safely copy flag value to non-shared variable.
    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    flag = _gnss_data_avail_flag;
    k_mutex_unlock(&_gnss_data_avail_mutex);

    if (!flag) {
        /*
         * If data isn't available, wait for it. But if timeout expires, exit
         * with failure.
         */

        LOG_INF("Waiting for GNSS data updates");

        // Wait for data availability, with configured timeout.

        status = k_sem_take(
            &_gnss_data_avail_sem, K_SECONDS(CONFIG_GNSS_DATA_TIMEOUT)
        );

        if (status < 0) {
            // On timeout expiry, exit with failure.
            LOG_ERR("Failed to obtain GNSS data updates (Timeout expired)");
            return -1;
        }
    }

    return 0;
}

void gnss_read (gnss_data_frame_t * data_frame) {
    // Block other threads from accessing shared resources.
    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    k_mutex_lock(&_gnss_data_frame_mutex, K_FOREVER);

    // Indicate that data is no longer available, by setting flag value to false
    // and resetting semaphore.
    k_sem_take(&_gnss_data_avail_sem, K_NO_WAIT);
    _gnss_data_avail_flag = false;

    // Copy data frame into output buffer.
    memcpy(data_frame, &_gnss_data_frame, sizeof(_gnss_data_frame));

    // Allow other threads to access shared resources.
    k_mutex_unlock(&_gnss_data_avail_mutex);
    k_mutex_unlock(&_gnss_data_frame_mutex);
}
