#include <stdbool.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <modem/lte_lc.h>

#include "data.h"

LOG_MODULE_REGISTER(lte, CONFIG_LTE_LOG_LEVEL);

static K_SEM_DEFINE(_lte_conn_avail_sem, 0, 1);
static K_MUTEX_DEFINE(_lte_conn_avail_mutex);
static bool _lte_conn_avail_flag = false;

static K_SEM_DEFINE(_lte_data_avail_sem, 0, 1);
static K_MUTEX_DEFINE(_lte_data_avail_mutex);
static bool _lte_data_avail_flag = false;

static K_MUTEX_DEFINE(_lte_data_frame_mutex);
static lte_data_frame_t _lte_data_frame = {
    .mode = {
        .valid = false,
        .mode = ""
    },
    .cell = {
        .valid = false,
        .id = 0,
        .tac = 0
    },
    .psm = {
        .valid = false,
        .tau = {
            .day = 0,
            .hour = 0,
            .min = 0,
            .sec = 0
        },
        .at = {
            .hour = 0,
            .min = 0,
            .sec = 0
        }
    },
    .edrx = {
        .valid = false,
        .mode = "",
        .edrx = {
            .hour = 0,
            .min = 0,
            .sec = 0,
            .msec = 0
        },
        .ptw = {
            .sec = 0,
            .msec = 0
        }
    }
};

static void _lte_handler (const struct lte_lc_evt * const evt) {
    switch (evt->type) {
        case LTE_LC_EVT_RRC_UPDATE:
            switch (evt->rrc_mode) {
                case LTE_LC_RRC_MODE_CONNECTED:
                    LOG_INF("Entered RRC connected mode");
                    break;
                case LTE_LC_RRC_MODE_IDLE:
                    LOG_INF("Entered RRC idle mode");
                    break;
            }
            break;
        case LTE_LC_EVT_NW_REG_STATUS:
            k_mutex_lock(&_lte_conn_avail_mutex, K_FOREVER);
            switch (evt->nw_reg_status) {
                case LTE_LC_NW_REG_SEARCHING:
                    LOG_INF("Searching for LTE network");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
                case LTE_LC_NW_REG_REGISTERED_HOME:
                    LOG_INF("Connected to LTE home network");
                    k_sem_give(&_lte_conn_avail_sem);
                    _lte_conn_avail_flag = true;
                    break;
                case LTE_LC_NW_REG_REGISTERED_ROAMING:
                    LOG_INF("Connected to LTE roaming network");
                    k_sem_give(&_lte_conn_avail_sem);
                    _lte_conn_avail_flag = true;
                    break;
                case LTE_LC_NW_REG_REGISTERED_EMERGENCY:
                    LOG_WRN("Connected to LTE emergency network");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
                case LTE_LC_NW_REG_NOT_REGISTERED:
                    LOG_WRN("Not connected to LTE network");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
                case LTE_LC_NW_REG_UNKNOWN:
                    LOG_WRN("LTE network connection status unknown");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
                case LTE_LC_NW_REG_REGISTRATION_DENIED:
                    LOG_WRN("LTE network connection denied");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
                case LTE_LC_NW_REG_UICC_FAIL:
                    LOG_WRN("LTE network connection failed due to UICC error");
                    k_sem_take(&_lte_conn_avail_sem, K_NO_WAIT);
                    _lte_conn_avail_flag = false;
                    break;
            }
            k_mutex_unlock(&_lte_conn_avail_mutex);
            break;
        case LTE_LC_EVT_LTE_MODE_UPDATE:
            k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
            k_mutex_lock(&_lte_data_frame_mutex, K_FOREVER);

            k_sem_give(&_lte_data_avail_sem);
            _lte_data_avail_flag = true;

            switch (evt->lte_mode) {
                case LTE_LC_LTE_MODE_NONE:
                    LOG_WRN("No active network mode");
                    _lte_data_frame.mode.valid = false;
                    _lte_data_frame.mode.mode = "";
                    break;
                case LTE_LC_LTE_MODE_LTEM:
                    LOG_INF("Switched to LTE-M network mode");
                    _lte_data_frame.mode.valid = true;
                    _lte_data_frame.mode.mode = "LTE-M";
                    break;
                case LTE_LC_LTE_MODE_NBIOT:
                    LOG_INF("Switched to NB-IoT network mode");
                    _lte_data_frame.mode.valid = true;
                    _lte_data_frame.mode.mode = "NB-IoT";
                    break;
            }

            k_mutex_unlock(&_lte_data_avail_mutex);
            k_mutex_unlock(&_lte_data_frame_mutex);
            break;
        case LTE_LC_EVT_CELL_UPDATE:
            k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
            k_mutex_lock(&_lte_data_frame_mutex, K_FOREVER);

            k_sem_give(&_lte_data_avail_sem);
            _lte_data_avail_flag = true;

            if ((int)(evt->cell.id) < 0 || (int)(evt->cell.tac) < 0) {
                _lte_data_frame.cell.valid = false;

                _lte_data_frame.cell.id = 0;
                _lte_data_frame.cell.tac = 0;

                LOG_WRN("Cell parameters unknown");
            } else {
                _lte_data_frame.cell.valid = true;

                _lte_data_frame.cell.id = evt->cell.id;
                _lte_data_frame.cell.tac = evt->cell.tac;

                LOG_INF(
                    "Cell parameters obtained: ID: %d, TAC: %d",
                    _lte_data_frame.cell.id, _lte_data_frame.cell.tac
                );
            }

            k_mutex_unlock(&_lte_data_avail_mutex);
            k_mutex_unlock(&_lte_data_frame_mutex);
            break;
        case LTE_LC_EVT_PSM_UPDATE:
            k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
            k_mutex_lock(&_lte_data_frame_mutex, K_FOREVER);

            k_sem_give(&_lte_data_avail_sem);
            _lte_data_avail_flag = true;

            if (evt->psm_cfg.tau < 0 || evt->psm_cfg.active_time < 0) {
                _lte_data_frame.psm.valid = false;

                _lte_data_frame.psm.tau.day = 0;
                _lte_data_frame.psm.tau.hour = 0;
                _lte_data_frame.psm.tau.min = 0;
                _lte_data_frame.psm.tau.sec = 0;

                _lte_data_frame.psm.at.hour = 0;
                _lte_data_frame.psm.at.min = 0;
                _lte_data_frame.psm.at.sec = 0;

                LOG_WRN("PSM parameters rejected");
            } else {
                _lte_data_frame.psm.valid = true;

                _lte_data_frame.psm.tau.day = (int)(
                    evt->psm_cfg.tau / 86400
                );
                _lte_data_frame.psm.tau.hour = (int)(
                    evt->psm_cfg.tau / 3600
                    - 24 * _lte_data_frame.psm.tau.day
                );
                _lte_data_frame.psm.tau.min = (int)(
                    evt->psm_cfg.tau / 60
                    - 1440 * _lte_data_frame.psm.tau.day
                    - 60 * _lte_data_frame.psm.tau.hour
                );
                _lte_data_frame.psm.tau.sec = (int)(
                    evt->psm_cfg.tau
                    - 86400 * _lte_data_frame.psm.tau.day
                    - 3600 * _lte_data_frame.psm.tau.hour
                    - 60 * _lte_data_frame.psm.tau.min
                );

                _lte_data_frame.psm.at.hour = (int)(
                    evt->psm_cfg.active_time / 3600
                );
                _lte_data_frame.psm.at.min = (int)(
                    evt->psm_cfg.active_time / 60
                    - 60 * _lte_data_frame.psm.at.hour
                );
                _lte_data_frame.psm.at.sec = (int)(
                    evt->psm_cfg.active_time
                    - 3600 * _lte_data_frame.psm.at.hour
                    - 60 * _lte_data_frame.psm.at.min
                );

                LOG_INF(
                    "PSM parameters granted: "
                    "TAU: %dd:%dh:%dm:%ds, AT: %dh:%dm:%ds",
                    _lte_data_frame.psm.tau.day, _lte_data_frame.psm.tau.hour,
                    _lte_data_frame.psm.tau.min, _lte_data_frame.psm.tau.sec,
                    _lte_data_frame.psm.at.hour, _lte_data_frame.psm.at.min,
                    _lte_data_frame.psm.at.sec
                );
            }

            k_mutex_unlock(&_lte_data_avail_mutex);
            k_mutex_unlock(&_lte_data_frame_mutex);
            break;
        case LTE_LC_EVT_EDRX_UPDATE:
            k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
            k_mutex_lock(&_lte_data_frame_mutex, K_FOREVER);

            k_sem_give(&_lte_data_avail_sem);
            _lte_data_avail_flag = true;

            switch (evt->edrx_cfg.mode) {
                case LTE_LC_LTE_MODE_NONE:
                    _lte_data_frame.edrx.valid = false;

                    _lte_data_frame.edrx.mode = "";

                    _lte_data_frame.edrx.edrx.hour = 0;
                    _lte_data_frame.edrx.edrx.min = 0;
                    _lte_data_frame.edrx.edrx.sec = 0;
                    _lte_data_frame.edrx.edrx.msec = 0;

                    _lte_data_frame.edrx.ptw.sec = 0;
                    _lte_data_frame.edrx.ptw.msec = 0;

                    LOG_WRN("eDRX parameters rejected");
                    break;
                case LTE_LC_LTE_MODE_LTEM:
                    _lte_data_frame.edrx.valid = true;

                    _lte_data_frame.edrx.mode = "LTE-M";

                    _lte_data_frame.edrx.edrx.hour = (int)(
                        evt->edrx_cfg.edrx / 3600
                    );
                    _lte_data_frame.edrx.edrx.min = (int)(
                        evt->edrx_cfg.edrx / 60
                        - 60 * _lte_data_frame.edrx.edrx.hour
                    );
                    _lte_data_frame.edrx.edrx.sec = (int)(
                        evt->edrx_cfg.edrx
                        - 3600 * _lte_data_frame.edrx.edrx.hour
                        - 60 * _lte_data_frame.edrx.edrx.min
                    );
                    _lte_data_frame.edrx.edrx.msec = (int)(
                        1000 * evt->edrx_cfg.edrx
                        - 3600000 * _lte_data_frame.edrx.edrx.hour
                        - 60000 * _lte_data_frame.edrx.edrx.min
                        - 1000 * _lte_data_frame.edrx.edrx.sec
                    );

                    _lte_data_frame.edrx.ptw.sec = (int)(
                        evt->edrx_cfg.ptw
                    );
                    _lte_data_frame.edrx.ptw.msec = (int)(
                        1000 * evt->edrx_cfg.ptw
                        - 1000 * _lte_data_frame.edrx.ptw.sec
                    );

                    LOG_INF(
                        "eDRX parameters granted: "
                        "Mode: LTE-M, eDRX: %dh:%dm:%d.%03ds, PTW: %d.%03ds",
                        _lte_data_frame.edrx.edrx.hour,
                        _lte_data_frame.edrx.edrx.min,
                        _lte_data_frame.edrx.edrx.sec,
                        _lte_data_frame.edrx.edrx.msec,
                        _lte_data_frame.edrx.ptw.sec,
                        _lte_data_frame.edrx.ptw.msec
                    );
                    break;
                case LTE_LC_LTE_MODE_NBIOT:
                    _lte_data_frame.edrx.valid = true;

                    _lte_data_frame.edrx.mode = "NB-IoT";

                    _lte_data_frame.edrx.edrx.hour = (int)(
                        evt->edrx_cfg.edrx / 3600
                    );
                    _lte_data_frame.edrx.edrx.min = (int)(
                        evt->edrx_cfg.edrx / 60
                        - 60 * _lte_data_frame.edrx.edrx.hour
                    );
                    _lte_data_frame.edrx.edrx.sec = (int)(
                        evt->edrx_cfg.edrx
                        - 3600 * _lte_data_frame.edrx.edrx.hour
                        - 60 * _lte_data_frame.edrx.edrx.min
                    );
                    _lte_data_frame.edrx.edrx.msec = (int)(
                        1000 * evt->edrx_cfg.edrx
                        - 3600000 * _lte_data_frame.edrx.edrx.hour
                        - 60000 * _lte_data_frame.edrx.edrx.min
                        - 1000 * _lte_data_frame.edrx.edrx.sec
                    );

                    _lte_data_frame.edrx.ptw.sec = (int)(
                        evt->edrx_cfg.ptw
                    );
                    _lte_data_frame.edrx.ptw.msec = (int)(
                        1000 * evt->edrx_cfg.ptw
                        - 1000 * _lte_data_frame.edrx.ptw.sec
                    );

                    LOG_INF(
                        "eDRX parameters granted: "
                        "Mode: NB-IoT, eDRX: %dh:%dm:%d.%03ds, PTW: %d.%03ds",
                        _lte_data_frame.edrx.edrx.hour,
                        _lte_data_frame.edrx.edrx.min,
                        _lte_data_frame.edrx.edrx.sec,
                        _lte_data_frame.edrx.edrx.msec,
                        _lte_data_frame.edrx.ptw.sec,
                        _lte_data_frame.edrx.ptw.msec
                    );
                    break;
            }

            k_mutex_unlock(&_lte_data_avail_mutex);
            k_mutex_unlock(&_lte_data_frame_mutex);
            break;
        default:
            break;
    }
}

int lte_init (void) {
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

    LOG_INF("Initializing LTE");

    status = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_POWER_OFF);
    if (status < 0) {
        LOG_ERR(
            "Failed to set modem to power off mode (%s)",
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

    if (IS_ENABLED(CONFIG_LTE_USE_PSM)) {
        status = lte_lc_psm_req(true);
        if (status < 0) {
            LOG_ERR(
                "Failed to enable PSM (%s)",
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
    } else {
        status = lte_lc_psm_req(false);
        if (status < 0) {
            LOG_ERR(
                "Failed to disable PSM (%s)",
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
    }

    if (IS_ENABLED(CONFIG_LTE_USE_EDRX)) {
        status = lte_lc_edrx_req(true);
        if (status < 0) {
            LOG_ERR(
                "Failed to enable eDRX (%s)",
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
    } else {
        status = lte_lc_edrx_req(false);
        if (status < 0) {
            LOG_ERR(
                "Failed to disable eDRX (%s)",
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
    }

    lte_lc_register_handler(_lte_handler);

    status = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_ACTIVATE_LTE);
    if (status < 0) {
        LOG_ERR(
            "Failed to set modem to LTE active mode (%s)",
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

void lte_deinit (void) {
    int status;

    LOG_WRN("Deinitializing modem");

    status = lte_lc_deinit();
    if (status < 0) {
        LOG_ERR(
            "Failed to deinitialize modem (%s)",
            strerror(-status)
        );
    }
}

bool lte_conn_avail (void) {
    bool flag;

    k_mutex_lock(&_lte_conn_avail_mutex, K_FOREVER);
    flag = _lte_conn_avail_flag;
    k_mutex_unlock(&_lte_conn_avail_mutex);

    return flag;
}

int lte_wait_conn_avail (void) {
    int status;
    bool flag;

    k_mutex_lock(&_lte_conn_avail_mutex, K_FOREVER);
    flag = _lte_conn_avail_flag;
    k_mutex_unlock(&_lte_conn_avail_mutex);

    if (!flag) {
        LOG_INF("Waiting for LTE connection");

        status = k_sem_take(
            &_lte_conn_avail_sem, K_SECONDS(CONFIG_LTE_CONN_TIMEOUT)
        );

        if (status < 0) {
            LOG_ERR("Failed to obtain LTE connection (Timeout expired)");
            return -1;
        }
    }

    return 0;
}

bool lte_data_avail (void) {
    bool flag;

    k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
    flag = _lte_data_avail_flag;
    k_mutex_unlock(&_lte_data_avail_mutex);

    return flag;
}

int lte_wait_data_avail (void) {
    int status;
    bool flag;

    k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
    flag = _lte_data_avail_flag;
    k_mutex_unlock(&_lte_data_avail_mutex);

    if (!flag) {
        LOG_INF("Waiting for LTE data updates");

        status = k_sem_take(
            &_lte_data_avail_sem, K_SECONDS(CONFIG_LTE_DATA_TIMEOUT)
        );

        if (status < 0) {
            LOG_ERR("Failed to obtain LTE data updates (Timeout expired)");
            return -1;
        }
    }

    return 0;
}

void lte_read (lte_data_frame_t * data_frame) {
    k_mutex_lock(&_lte_data_avail_mutex, K_FOREVER);
    k_mutex_lock(&_lte_data_frame_mutex, K_FOREVER);

    k_sem_take(&_lte_data_avail_sem, K_NO_WAIT);
    _lte_data_avail_flag = false;

    memcpy(data_frame, &_lte_data_frame, sizeof(_lte_data_frame));

    k_mutex_unlock(&_lte_data_avail_mutex);
    k_mutex_unlock(&_lte_data_frame_mutex);
}
