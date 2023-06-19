/** @mainpage   nRF9160 Logger
 *
 *  @brief      Configurable logger based on the Actinius Icarus v2.
 *
 *  The *nRF9160 Logger* is a configurable logging device based on the Actinius
 *  Icarus v2 development board, which features the nRF9160 SiP. This logger can
 *  be configured to regularly upload dummy data, LTE network information, or
 *  GNSS fixes to an online database. For uploading data, the device accesses
 *  the internet over the LTE-M/NB-IoT network.
 */

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "data.h"
#include "dummy.h"
#include "lte.h"
#include "gnss.h"
#include "rest.h"

// Register module for logging.
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

void app_dummy_logger (void) {
    int status; // Return status for API calls.

    dummy_data_frame_t dummy_data_frame;            // Data frame.
    char dummy_json[CONFIG_MAIN_DUMMY_BUF_SIZE];    // JSON buffer.

    /*
     * Program loop. Each cycle begins with an interval during which the device
     * remains in sleep mode. After this, it obtains a dummy data frame and
     * encodes it in JSON format. Finally, it connects to the LTE network,
     * uploads the data frame, and then disconnects from the network. If errors
     * occur anywhere in this process, the LTE module is deactivated and the
     * cycle is restarted.
     */

    while (true) {
        /*
         * Enter sleep mode for the configured time interval.
         */

        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        /*
         * Obtain data frame and encode it in JSON format. If an error occurs in
         * this process, restart the cycle.
         */

        LOG_INF("Obtaining dummy data");

        // Read data frame.
        dummy_read(&dummy_data_frame);

        // Encode data frame in JSON format.

        status = data_dummy_data_frame_to_json(
            &dummy_data_frame, dummy_json, sizeof(dummy_json)
        );

        if (status < 0) {
            // On error, restart cycle.
            continue;
        }

        /*
         * Connect to LTE network, upload data frame, and then disconnect from
         * network. If an error occurs anywhere in this process, deactivate LTE
         * and restart the cycle.
         */

        LOG_INF("Activating LTE system");

        // Activate LTE.
        status = lte_init();
        if (status < 0) {
            // On error, restart cycle.
            continue;
        }

        // Wait for connection to establish.
        status = lte_wait_conn_avail();
        if (status < 0) {
            // On error, deactivate LTE and restart cycle.
            lte_deinit();
            continue;
        }

        // Upload data frame.
        LOG_INF("Uploading dummy data");
        rest_post(CONFIG_MAIN_DUMMY_UPLOAD_URL, dummy_json);

        // Deactivate LTE.
        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void app_lte_logger (void) {
    int status; // Return status for API calls.

    lte_data_frame_t lte_data_frame;            // Data frame.
    char lte_json[CONFIG_MAIN_LTE_BUF_SIZE];    // JSON buffer.

    /*
     * Program loop. Each cycle begins with an interval during which the device
     * remains in sleep mode. After this, it connects to the LTE network. Once
     * connected, it repeatedly waits for updates to network-related
     * information. Every time an update is received, an LTE data frame is
     * obtained, encoded in JSON format, and uploaded. If no update is received
     * before a timeout expires, the device disconnects from the network. If an
     * error occurs anywhere in this process, the LTE module is deactivated and
     * the cycle is restarted.
     */

    while (true) {
        /*
         * Enter sleep mode for the configured time interval.
         */

        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        /*
         * Connect to LTE network. After this, repeatedly wait for data frames
         * and upload them as they are received. If a timeout expires,
         * disconnect from the network. If an error occurs anywhere in this
         * process, deactivate LTE and restart the cycle.
         */

        LOG_INF("Activating LTE system");

        // Activate LTE.
        status = lte_init();
        if (status < 0) {
            // On error, restart cycle.
            continue;
        }

        // Wait for connection to establish.
        status = lte_wait_conn_avail();
        if (status < 0) {
            // On error, deactivate LTE and restart cycle.
            lte_deinit();
            continue;
        }

        // Repeatedly obtain and upload data frames.
        while (true) {
            LOG_INF("Obtaining LTE data");

            // Wait for data frame.
            status = lte_wait_data_avail();
            if (status < 0) {
                // On timeout expiry, exit upload cycle.
                break;
            }

            // Read data frame.
            lte_read(&lte_data_frame);

            // Encode data frame in JSON format.

            status = data_lte_data_frame_to_json(
                &lte_data_frame, lte_json, sizeof(lte_json)
            );

            if (status < 0) {
                // On error, restart upload cycle.
                continue;
            }

            // Upload data frame.
            LOG_INF("Uploading LTE data");
            rest_post(CONFIG_MAIN_LTE_UPLOAD_URL, lte_json);
        }

        // Deactivate LTE.
        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void app_gnss_logger (void) {
    int status; // Return status for API calls.

    gnss_data_frame_t gnss_data_frame;          // Data frame.
    char gnss_json[CONFIG_MAIN_GNSS_BUF_SIZE];  // JSON buffer.

    /*
     * Program loop. Each cycle begins with an interval during which the device
     * remains in sleep mode. After this, it starts GNSS reception and waits for
     * a fix. If a timeout expires, the GNSS module is deactivated and the cycle
     * is restarted. If a fix was achieved, a data frame is obtained and encoded
     * in JSON format. The device then deactivates the GNSS module and connects
     * to the LTE network, uploads the data frame, and then disconnects from the
     * network. If an error occurs anywhere in this process, the GNSS and LTE
     * modules are both deactivated, and the cycle is restarted.
     */

    while (true) {
        /*
         * Enter sleep mode for the configured time interval.
         */

        LOG_INF("Entered sleep mode");
        k_sleep(K_SECONDS(CONFIG_MAIN_SLEEP_TIME));

        /*
         * Start GNSS reception and wait for a data frame. If a timeout expires,
         * deactivate GNSS and restart the cycle. Otherwise, deactivate GNSS,
         * encode the data frame in JSON format and proceed. If an error occurs
         * anywhere in this process, deactivate GNSS and restart the cycle.
         */

        LOG_INF("Activating GNSS system");

        // Activate GNSS.
        status = gnss_init();
        if (status < 0) {
            // On error, restart cycle.
            continue;
        }

        LOG_INF("Obtaining GNSS data");

        // Wait for data frame.
        status = gnss_wait_data_avail();
        if (status < 0) {
            // On error, deactivate GNSS and restart cycle.
            gnss_deinit();
            continue;
        }

        // Read data frame.
        gnss_read(&gnss_data_frame);

        // Encode data frame in JSON format.

        status = data_gnss_data_frame_to_json(
            &gnss_data_frame, gnss_json, sizeof(gnss_json)
        );

        if (status < 0) {
            // On error, deactivate GNSS and restart cycle.
            gnss_deinit();
            continue;
        }

        // Deactivate GNSS.
        LOG_INF("Deactivating GNSS system");
        gnss_deinit();

        /*
         * Connect to LTE network, upload data frame, and then disconnect from
         * the network. If an error occurs anywhere in this process, deactivate
         * LTE and restart the cycle.
         */

        LOG_INF("Activating LTE system");

        // Activate LTE.
        status = lte_init();
        if (status < 0) {
            // On error, restart cycle.
            continue;
        }

        // Wait for connection to establish.
        status = lte_wait_conn_avail();
        if (status < 0) {
            // On error, deactivate LTE and restart cycle.
            lte_deinit();
            continue;
        }

        // Upload data frame.
        LOG_INF("Uploading GNSS data");
        rest_post(CONFIG_MAIN_GNSS_UPLOAD_URL, gnss_json);

        // Deactivate LTE.
        LOG_INF("Deactivating LTE system");
        lte_deinit();
    }
}

void main (void) {
    // Check configuration and start corresponding application.
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
