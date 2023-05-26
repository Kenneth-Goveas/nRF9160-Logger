#ifndef __DATA_H__
#define __DATA_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    const char * field1;
    const char * field2;
    const char * field3;
    const char * field4;
} dummy_data_frame_t;

typedef struct {
    bool valid;
    const char * mode;
} lte_data_frame_mode_t;

typedef struct {
    bool valid;
    int id;
    int tac;
} lte_data_frame_cell_t;

typedef struct {
    int day;
    int hour;
    int min;
    int sec;
} lte_data_frame_psm_tau_t;

typedef struct {
    int hour;
    int min;
    int sec;
} lte_data_frame_psm_at_t;

typedef struct {
    bool valid;
    lte_data_frame_psm_tau_t tau;
    lte_data_frame_psm_at_t at;
} lte_data_frame_psm_t;

typedef struct {
    int hour;
    int min;
    int sec;
    int msec;
} lte_data_frame_edrx_edrx_t;

typedef struct {
    int sec;
    int msec;
} lte_data_frame_edrx_ptw_t;

typedef struct {
    bool valid;
    const char * mode;
    lte_data_frame_edrx_edrx_t edrx;
    lte_data_frame_edrx_ptw_t ptw;
} lte_data_frame_edrx_t;

typedef struct {
    lte_data_frame_mode_t mode;
    lte_data_frame_cell_t cell;
    lte_data_frame_psm_t psm;
    lte_data_frame_edrx_t edrx;
} lte_data_frame_t;

typedef struct {
    const char * dir;
    int deg;
    int min;
    int sec;
    int msec;
} gnss_data_frame_loc_lat_t;

typedef struct {
    const char * dir;
    int deg;
    int min;
    int sec;
    int msec;
} gnss_data_frame_loc_lon_t;

typedef struct {
    bool valid;
    gnss_data_frame_loc_lat_t lat;
    gnss_data_frame_loc_lon_t lon;
} gnss_data_frame_loc_t;

typedef struct {
    bool valid;
    int year;
    int mon;
    int day;
} gnss_data_frame_date_t;

typedef struct {
    bool valid;
    int hour;
    int min;
    int sec;
    int msec;
} gnss_data_frame_time_t;

typedef struct {
    gnss_data_frame_loc_t loc;
    gnss_data_frame_date_t date;
    gnss_data_frame_time_t time;
} gnss_data_frame_t;

int data_dummy_data_frame_to_json (
    dummy_data_frame_t * data_frame, char * json, size_t len
);

int data_lte_data_frame_to_json (
    lte_data_frame_t * data_frame, char * json, size_t len
);

int data_gnss_data_frame_to_json (
    gnss_data_frame_t * data_frame, char * json, size_t len
);

#endif
