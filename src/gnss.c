#include <stdbool.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <modem/lte_lc.h>
#include <nrf_modem_gnss.h>

#include "data.h"

LOG_MODULE_REGISTER(gnss, CONFIG_GNSS_LOG_LEVEL);

static K_SEM_DEFINE(_gnss_data_avail_sem, 0, 1);
static K_MUTEX_DEFINE(_gnss_data_avail_mutex);
static bool _gnss_data_avail_flag = false;

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
    int status;
    struct nrf_modem_gnss_pvt_data_frame pvt;

    switch (evt) {
        case NRF_MODEM_GNSS_EVT_PVT:
            status = nrf_modem_gnss_read(
                &pvt, sizeof(pvt), NRF_MODEM_GNSS_DATA_PVT
            );
            if (status < 0) {
                LOG_ERR(
                    "Failed to parse GNSS fix (%s)",
                    strerror(-status)
                );
            } else if (pvt.flags & NRF_MODEM_GNSS_PVT_FLAG_FIX_VALID) {
                k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
                k_mutex_lock(&_gnss_data_frame_mutex, K_FOREVER);

                k_sem_give(&_gnss_data_avail_sem);
                _gnss_data_avail_flag = true;

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

                k_mutex_unlock(&_gnss_data_avail_mutex);
                k_mutex_unlock(&_gnss_data_frame_mutex);
            }
            break;
        default:
            break;
    }
}

int gnss_init (void) {
    int status;

    LOG_INF("Initializing modem");

    status = lte_lc_init();
    if (status < 0) {
        LOG_ERR(
            "Failed to initialize modem (%s)",
            strerror(-status)
        );
        return -1;
    }

    LOG_INF("Initializing GNSS");

    status = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_ACTIVATE_GNSS);
    if (status < 0) {
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

    status = nrf_modem_gnss_fix_interval_set(0);
    if (status < 0) {
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

    status = nrf_modem_gnss_fix_retry_set(0);
    if (status < 0) {
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

    nrf_modem_gnss_event_handler_set(_gnss_handler);

    LOG_INF("Starting GNSS");

    status = nrf_modem_gnss_start();
    if (status < 0) {
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
    int status;

    LOG_WRN("Stopping GNSS");

    status = nrf_modem_gnss_stop();
    if (status < 0) {
        LOG_ERR(
            "Failed to stop GNSS (%s)",
            strerror(-status)
        );
    }

    LOG_WRN("Deinitializing modem");

    status = lte_lc_deinit();
    if (status < 0) {
        LOG_ERR(
            "Failed to deinitialize modem (%s)",
            strerror(-status)
        );
    }
}

bool gnss_data_avail (void) {
    bool flag;

    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    flag = _gnss_data_avail_flag;
    k_mutex_unlock(&_gnss_data_avail_mutex);

    return flag;
}

int gnss_wait_data_avail (void) {
    int status;
    bool flag;

    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    flag = _gnss_data_avail_flag;
    k_mutex_unlock(&_gnss_data_avail_mutex);

    if (!flag) {
        LOG_INF("Waiting for GNSS data updates");

        status = k_sem_take(
            &_gnss_data_avail_sem, K_SECONDS(CONFIG_GNSS_DATA_TIMEOUT)
        );

        if (status < 0) {
            LOG_ERR("Failed to obtain GNSS data updates (Timeout expired)");
            return -1;
        }
    }

    return 0;
}

void gnss_read (gnss_data_frame_t * data_frame) {
    k_mutex_lock(&_gnss_data_avail_mutex, K_FOREVER);
    k_mutex_lock(&_gnss_data_frame_mutex, K_FOREVER);

    k_sem_take(&_gnss_data_avail_sem, K_NO_WAIT);
    _gnss_data_avail_flag = false;

    memcpy(data_frame, &_gnss_data_frame, sizeof(_gnss_data_frame));

    k_mutex_unlock(&_gnss_data_avail_mutex);
    k_mutex_unlock(&_gnss_data_frame_mutex);
}
