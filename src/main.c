#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "data.h"
#include "dummy.h"
#include "lte.h"
#include "gnss.h"
#include "rest.h"

LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

void app_dummy_logger (void) {
    int status;

    dummy_data_frame_t dummy_data_frame;
    char dummy_json[CONFIG_MAIN_DUMMY_BUF_SIZE];

    while (true) {
        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        LOG_INF("Obtaining dummy data");

        dummy_read(&dummy_data_frame);

        status = data_dummy_data_frame_to_json(
            &dummy_data_frame, dummy_json, sizeof(dummy_json)
        );

        if (status < 0) {
            continue;
        }

        LOG_INF("Activating LTE system");

        status = lte_init();
        if (status < 0) {
            continue;
        }

        status = lte_wait_conn_avail();
        if (status < 0) {
            lte_deinit();
            continue;
        }

        LOG_INF("Uploading dummy data");
        rest_post(CONFIG_MAIN_DUMMY_UPLOAD_URL, dummy_json);

        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void app_lte_logger (void) {
    int status;

    lte_data_frame_t lte_data_frame;
    char lte_json[CONFIG_MAIN_LTE_BUF_SIZE];

    while (true) {
        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        LOG_INF("Activating LTE system");

        status = lte_init();
        if (status < 0) {
            continue;
        }

        status = lte_wait_conn_avail();
        if (status < 0) {
            lte_deinit();
            continue;
        }

        while (true) {
            LOG_INF("Obtaining LTE data");

            status = lte_wait_data_avail();
            if (status < 0) {
                break;
            }

            lte_read(&lte_data_frame);

            status = data_lte_data_frame_to_json(
                &lte_data_frame, lte_json, sizeof(lte_json)
            );

            if (status < 0) {
                continue;
            }

            LOG_INF("Uploading LTE data");
            rest_post(CONFIG_MAIN_LTE_UPLOAD_URL, lte_json);
        }

        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void app_gnss_logger (void) {
    int status;

    gnss_data_frame_t gnss_data_frame;
    char gnss_json[CONFIG_MAIN_GNSS_BUF_SIZE];

    while (true) {
        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        LOG_INF("Activating GNSS system");

        status = gnss_init();
        if (status < 0) {
            continue;
        }

        LOG_INF("Obtaining GNSS data");

        status = gnss_wait_data_avail();
        if (status < 0) {
            gnss_deinit();
            continue;
        }

        gnss_read(&gnss_data_frame);

        status = data_gnss_data_frame_to_json(
            &gnss_data_frame, gnss_json, sizeof(gnss_json)
        );

        if (status < 0) {
            gnss_deinit();
            continue;
        }

        LOG_INF("Deactivating GNSS system");
        gnss_deinit();

        LOG_INF("Activating LTE system");

        status = lte_init();
        if (status < 0) {
            continue;
        }

        status = lte_wait_conn_avail();
        if (status < 0) {
            lte_deinit();
            continue;
        }

        LOG_INF("Uploading GNSS data");
        rest_post(CONFIG_MAIN_GNSS_UPLOAD_URL, gnss_json);

        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void main (void) {
    if (IS_ENABLED(CONFIG_MAIN_DATA_TYPE_DUMMY)) {
        LOG_INF("Starting dummy logging application");
        app_dummy_logger();
    } else if (IS_ENABLED(CONFIG_MAIN_DATA_TYPE_LTE)) {
        LOG_INF("Starting LTE logging application");
        app_lte_logger();
    } else if (IS_ENABLED(CONFIG_MAIN_DATA_TYPE_GNSS)) {
        LOG_INF("Starting GNSS logging application");
        app_gnss_logger();
    }
}
