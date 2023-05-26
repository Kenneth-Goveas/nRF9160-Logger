#include <stddef.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/data/json.h>

#include "data.h"

LOG_MODULE_REGISTER(data, CONFIG_DATA_LOG_LEVEL);

static const struct json_obj_descr _dummy_data_frame_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        dummy_data_frame_t, "field1", field1,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        dummy_data_frame_t, "field2", field2,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        dummy_data_frame_t, "field3", field3,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        dummy_data_frame_t, "field4", field4,
        JSON_TOK_STRING
    )
};

static const struct json_obj_descr _lte_data_frame_mode_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_mode_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_mode_t, "mode", mode,
        JSON_TOK_STRING
    )
};

static const struct json_obj_descr _lte_data_frame_cell_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_cell_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_cell_t, "id", id,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_cell_t, "tac", tac,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _lte_data_frame_psm_tau_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_tau_t, "days", day,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_tau_t, "hours", hour,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_tau_t, "minutes", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_tau_t, "seconds", sec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _lte_data_frame_psm_at_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_at_t, "hours", hour,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_at_t, "minutes", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_at_t, "seconds", sec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _lte_data_frame_psm_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_psm_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_psm_t, "tau", tau,
        _lte_data_frame_psm_tau_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_psm_t, "at", at,
        _lte_data_frame_psm_at_descr
    )
};

static const struct json_obj_descr _lte_data_frame_edrx_edrx_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_edrx_t, "hours", hour,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_edrx_t, "minutes", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_edrx_t, "seconds", sec,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_edrx_t, "milliseconds", msec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _lte_data_frame_edrx_ptw_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_ptw_t, "seconds", sec,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_ptw_t, "milliseconds", msec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _lte_data_frame_edrx_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        lte_data_frame_edrx_t, "mode", mode,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_edrx_t, "edrx", edrx,
        _lte_data_frame_edrx_edrx_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_edrx_t, "ptw", ptw,
        _lte_data_frame_edrx_ptw_descr
    )
};

static const struct json_obj_descr _lte_data_frame_descr [] = {
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_t, "mode", mode,
        _lte_data_frame_mode_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_t, "cell", cell,
        _lte_data_frame_cell_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_t, "psm", psm,
        _lte_data_frame_psm_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        lte_data_frame_t, "edrx", edrx,
        _lte_data_frame_edrx_descr
    )
};

static const struct json_obj_descr _gnss_data_frame_loc_lat_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lat_t, "direction", dir,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lat_t, "degrees", deg,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lat_t, "minutes", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lat_t, "seconds", sec,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lat_t, "milliseconds", msec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _gnss_data_frame_loc_lon_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lon_t, "direction", dir,
        JSON_TOK_STRING
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lon_t, "degrees", deg,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lon_t, "minutes", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lon_t, "seconds", sec,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_lon_t, "milliseconds", msec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _gnss_data_frame_loc_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_loc_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        gnss_data_frame_loc_t, "latitude", lat,
        _gnss_data_frame_loc_lat_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        gnss_data_frame_loc_t, "longitude", lon,
        _gnss_data_frame_loc_lon_descr
    )
};

static const struct json_obj_descr _gnss_data_frame_date_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_date_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_date_t, "year", year,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_date_t, "month", mon,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_date_t, "day", day,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _gnss_data_frame_time_descr [] = {
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_time_t, "valid", valid,
        JSON_TOK_TRUE
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_time_t, "hour", hour,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_time_t, "minute", min,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_time_t, "second", sec,
        JSON_TOK_NUMBER
    ),
    JSON_OBJ_DESCR_PRIM_NAMED(
        gnss_data_frame_time_t, "millisecond", msec,
        JSON_TOK_NUMBER
    )
};

static const struct json_obj_descr _gnss_data_frame_descr [] = {
    JSON_OBJ_DESCR_OBJECT_NAMED(
        gnss_data_frame_t, "location", loc,
        _gnss_data_frame_loc_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        gnss_data_frame_t, "date", date,
        _gnss_data_frame_date_descr
    ),
    JSON_OBJ_DESCR_OBJECT_NAMED(
        gnss_data_frame_t, "time", time,
        _gnss_data_frame_time_descr
    )
};

int data_dummy_data_frame_to_json (
    dummy_data_frame_t * data_frame, char * json, size_t len
) {
    int status;

    memset(json, 0, len);

    LOG_INF("Encoding dummy data frame into JSON format");

    status = json_obj_encode_buf(
        _dummy_data_frame_descr, ARRAY_SIZE(_dummy_data_frame_descr),
        data_frame, json, len
    );

    if (status < 0) {
        LOG_ERR(
            "Failed to encode dummy data frame into JSON format (%s)",
            strerror(-status)
        );
        return -1;
    }

    return 0;
}

int data_lte_data_frame_to_json (
    lte_data_frame_t * data_frame, char * json, size_t len
) {
    int status;

    memset(json, 0, len);

    LOG_INF("Encoding LTE data frame into JSON format");

    status = json_obj_encode_buf(
        _lte_data_frame_descr, ARRAY_SIZE(_lte_data_frame_descr),
        data_frame, json, len
    );

    if (status < 0) {
        LOG_ERR(
            "Failed to encode LTE data frame into JSON format (%s)",
            strerror(-status)
        );
        return -1;
    }

    return 0;
}

int data_gnss_data_frame_to_json (
    gnss_data_frame_t * data_frame, char * json, size_t len
) {
    int status;

    memset(json, 0, len);

    LOG_INF("Encoding GNSS data frame into JSON format");

    status = json_obj_encode_buf(
        _gnss_data_frame_descr, ARRAY_SIZE(_gnss_data_frame_descr),
        data_frame, json, len
    );

    if (status < 0) {
        LOG_ERR(
            "Failed to encode GNSS data frame into JSON format (%s)",
            strerror(-status)
        );
        return -1;
    }

    return 0;
}
