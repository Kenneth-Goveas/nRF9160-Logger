/** @defgroup   data Data
 *
 *  @brief      Data frame processing.
 *
 *  This module defines the various data frame structures required by the
 *  application and is responsible for encoding them in JSON format. The three
 *  types of data frames used are dummy_data_frame_t, lte_data_frame_t, and
 *  gnss_data_frame_t, which can be encoded in JSON format by calling
 *  data_dummy_data_frame_to_json(), data_lte_data_frame_to_json(), and
 *  data_gnss_data_frame_to_json() respectively.
 */

#ifndef __DATA_H__
#define __DATA_H__

#include <stdbool.h>
#include <stddef.h>

/** @ingroup    data
 *
 *  @brief      Dummy data frame.
 *
 *  This data frame consists of dummy data solely for the sake of uploading. It
 *  does not, itself, contain any real useful information.
 */

typedef struct {
    const char * field1;    //!< Dummy string 1.
    const char * field2;    //!< Dummy string 2.
    const char * field3;    //!< Dummy string 3.
    const char * field4;    //!< Dummy string 4.
} dummy_data_frame_t;

/** @ingroup    data
 *
 *  @brief      LTE network mode.
 *
 *  This structure contains the currently active LTE network mode. It is
 *  embedded in the parent lte_data_frame_t structure that represents the
 *  complete LTE data frame.
 */

typedef struct {
    bool valid;         //!< Data valid.
    const char * mode;  //!< String representation of active network mode.
} lte_data_frame_mode_t;

/** @ingroup    data
 *
 *  @brief      LTE cell information.
 *
 *  This structure contains the current LTE cell parameters. It is embedded in
 *  the parent lte_data_frame_t structure that represents the complete LTE data
 *  frame.
 */

typedef struct {
    bool valid; //!< Data valid.
    int id;     //!< Cell ID.
    int tac;    //!< Tracking area code.
} lte_data_frame_cell_t;

/** @ingroup    data
 *
 *  @brief      LTE PSM periodic TAU interval.
 *
 *  This structure contains the LTE PSM periodic TAU interval. It is embedded in
 *  the parent lte_data_frame_psm_t structure that represents the complete LTE
 *  PSM configuration.
 */

typedef struct {
    int day;    //!< Days.
    int hour;   //!< Hours.
    int min;    //!< Minutes.
    int sec;    //!< Seconds.
} lte_data_frame_psm_tau_t;

/** @ingroup    data
 *
 *  @brief      LTE PSM active time interval.
 *
 *  This structure contains the LTE PSM active time interval. It is embedded in
 *  the parent lte_data_frame_psm_t structure that represents the complete LTE
 *  PSM configuration.
 */

typedef struct {
    int hour;   //!< Hours.
    int min;    //!< Minutes.
    int sec;    //!< Seconds.
} lte_data_frame_psm_at_t;

/** @ingroup    data
 *
 *  @brief      LTE PSM configuration.
 *
 *  This structure contains the current LTE PSM configuration. It is embedded in
 *  the parent lte_data_frame_t structure that represents the complete LTE data
 *  frame.
 */

typedef struct {
    bool valid;                     //!< Data valid.
    lte_data_frame_psm_tau_t tau;   //!< Periodic TAU interval.
    lte_data_frame_psm_at_t at;     //!< Active time interval.
} lte_data_frame_psm_t;

/** @ingroup    data
 *
 *  @brief      LTE eDRX time interval.
 *
 *  This structure contains the LTE eDRX time interval. It is embedded in the
 *  parent lte_data_frame_edrx_t structure that represents the complete LTE eDRX
 *  configuration.
 */

typedef struct {
    int hour;   //!< Hours.
    int min;    //!< Minutes.
    int sec;    //!< Seconds.
    int msec;   //!< Milliseconds.
} lte_data_frame_edrx_edrx_t;

/** @ingroup    data
 *
 *  @brief      LTE eDRX paging time window.
 *
 *  This structure contains the LTE eDRX paging time window. It is embedded in
 *  the parent lte_data_frame_edrx_t structure that represents the complete LTE
 *  eDRX configuration.
 */

typedef struct {
    int sec;    //!< Seconds.
    int msec;   //!< Milliseconds.
} lte_data_frame_edrx_ptw_t;

/** @ingroup    data
 *
 *  @brief      LTE eDRX configuration.
 *
 *  This structure contains the current LTE eDRX configuration. It is embedded
 *  in the parent lte_data_frame_t structure that represents the complete LTE
 *  data frame.
 */

typedef struct {
    bool valid;                         //!< Data valid.
    const char * mode;                  //!< Associated network mode.
    lte_data_frame_edrx_edrx_t edrx;    //!< Time interval.
    lte_data_frame_edrx_ptw_t ptw;      //!< Paging time window.
} lte_data_frame_edrx_t;

/** @ingroup    data
 *
 *  @brief      LTE data frame.
 *
 *  This data frame contains LTE network-related information obtained through
 *  notifications received from the modem.
 */

typedef struct {
    lte_data_frame_mode_t mode; //!< Network mode.
    lte_data_frame_cell_t cell; //!< Cell information.
    lte_data_frame_psm_t psm;   //!< PSM configuration.
    lte_data_frame_edrx_t edrx; //!< eDRX configuration.
} lte_data_frame_t;

/** @ingroup    data
 *
 *  @brief      GNSS latitude.
 *
 *  This structure contains the GNSS latitude. It is embedded in the parent
 *  gnss_data_frame_loc_t structure that represents the complete GNSS location.
 */

typedef struct {
    const char * dir;   //!< Geographic direction.
    int deg;            //!< Degrees.
    int min;            //!< Minutes.
    int sec;            //!< Seconds.
    int msec;           //!< Milliseconds.
} gnss_data_frame_loc_lat_t;

/** @ingroup    data
 *
 *  @brief      GNSS longitude.
 *
 *  This structure contains the GNSS longitude. It is embedded in the parent
 *  gnss_data_frame_loc_t structure that represents the complete GNSS location.
 */

typedef struct {
    const char * dir;   //!< Geographic direction.
    int deg;            //!< Degrees.
    int min;            //!< Minutes.
    int sec;            //!< Seconds.
    int msec;           //!< Milliseconds.
} gnss_data_frame_loc_lon_t;

/** @ingroup    data
 *
 *  @brief      GNSS location.
 *
 *  This structure contains the GNSS location. It is embedded in the parent
 *  gnss_data_frame_t structure that represents the complete GNSS data frame.
 */

typedef struct {
    bool valid;                     //!< Data valid.
    gnss_data_frame_loc_lat_t lat;  //!< Latitude.
    gnss_data_frame_loc_lon_t lon;  //!< Longitude.
} gnss_data_frame_loc_t;

/** @ingroup    data
 *
 *  @brief      GNSS date.
 *
 *  This structure contains the GNSS date. It is embedded in the parent
 *  gnss_data_frame_t structure that represents the complete GNSS data frame.
 */

typedef struct {
    bool valid; //!< Data valid.
    int year;   //!< Year.
    int mon;    //!< Month.
    int day;    //!< Day.
} gnss_data_frame_date_t;

/** @ingroup    data
 *
 *  @brief      GNSS time.
 *
 *  This structure contains the GNSS time. It is embedded in the parent
 *  gnss_data_frame_t structure that represents the complete GNSS data frame.
 */

typedef struct {
    bool valid; //!< Data valid.
    int hour;   //!< Hour.
    int min;    //!< Minute.
    int sec;    //!< Second.
    int msec;   //!< Millisecond.
} gnss_data_frame_time_t;

/** @ingroup    data
 *
 *  @brief      GNSS data frame.
 *
 *  This data frame contains information from the latest obtained GNSS fix.
 */

typedef struct {
    gnss_data_frame_loc_t loc;      //!< Location.
    gnss_data_frame_date_t date;    //!< Date.
    gnss_data_frame_time_t time;    //!< Time.
} gnss_data_frame_t;

/** @ingroup    data
 *
 *  @brief      Encode dummy data frame in JSON format.
 *
 *  Encodes the given dummy data frame structure in a JSON-format string.
 *
 *  @param      data_frame  Pointer to dummy data frame to be encoded.
 *  @param      json        Pointer to buffer into which encoded string should
 *                          be written, along with terminating null byte.
 *  @param      len         Length of buffer provided for encoded string.
 *
 *  @retval     0           Success.
 *  @retval     -1          Failure.
 */

int data_dummy_data_frame_to_json (
    dummy_data_frame_t * data_frame, char * json, size_t len
);

/** @ingroup    data
 *
 *  @brief      Encode LTE data frame in JSON format.
 *
 *  Encodes the given LTE data frame structure in a JSON-format string.
 *
 *  @param      data_frame  Pointer to LTE data frame to be encoded.
 *  @param      json        Pointer to buffer into which encoded string should
 *                          be written, along with terminating null byte.
 *  @param      len         Length of buffer provided for encoded string.
 *
 *  @retval     0           Success.
 *  @retval     -1          Failure.
 */

int data_lte_data_frame_to_json (
    lte_data_frame_t * data_frame, char * json, size_t len
);

/** @ingroup    data
 *
 *  @brief      Encode GNSS data frame in JSON format.
 *
 *  Encodes the given GNSS data frame structure in a JSON-format string.
 *
 *  @param      data_frame  Pointer to GNSS data frame to be encoded.
 *  @param      json        Pointer to buffer into which encoded string should
 *                          be written, along with terminating null byte.
 *  @param      len         Length of buffer provided for encoded string.
 *
 *  @retval     0           Success.
 *  @retval     -1          Failure.
 */

int data_gnss_data_frame_to_json (
    gnss_data_frame_t * data_frame, char * json, size_t len
);

#endif
