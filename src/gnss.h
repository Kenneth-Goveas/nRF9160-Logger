/** @defgroup   gnss GNSS
 *
 *  @brief      GNSS reception.
 *
 *  This module controls the modem GNSS interface. The interface can be
 *  activated by calling gnss_init(). While active, the interface continuously
 *  tries to obtain a GNSS fix. Once a fix is obtained, no further fix attempts
 *  are made. Every time a fix is obtained, an internally stored GNSS data frame
 *  is updated with the newly received data. The availability of such data can
 *  be checked with gnss_data_avail(). The GNSS data frame can be read by
 *  calling gnss_read(). When not required, the GNSS interface can be
 *  deactivated by calling gnss_deinit().
 */

#ifndef __GNSS_H__
#define __GNSS_H__

#include <stdbool.h>

#include "data.h"

/** @ingroup    gnss
 *
 *  @brief      Initialize GNSS interface.
 *
 *  Powers on and initializes the modem GNSS interface. The LTE interface
 *  remains powered off.
 *
 *  @note       This function must not be called while the LTE interface is
 *              active.
 *
 *  @retval     0   Success.
 *  @retval     -1  Failure. Interface is powered back off in this case.
 */

int gnss_init (void);

/** @ingroup    gnss
 *
 *  @brief      Deinitialize GNSS interface.
 *
 *  Deinitializes and powers off the modem GNSS interface.
 */

void gnss_deinit (void);

/** @ingroup    gnss
 *
 *  @brief      Check for unread GNSS data.
 *
 *  Checks if the internally stored GNSS data frame contains unread data.
 *
 *  @retval     true    GNSS data available.
 *  @retval     false   GNSS data unavailable.
 */

bool gnss_data_avail (void);

/** @ingroup    gnss
 *
 *  @brief      Wait for unread GNSS data.
 *
 *  Waits until unread data become available in the internally stored GNSS data
 *  frame. This call fails if the configured GNSS data update timeout expires.
 *
 *  @retval     0   Success.
 *  @retval     -1  Failure.
 */

int gnss_wait_data_avail (void);

/** @ingroup    gnss
 *
 *  @brief      Read GNSS data frame.
 *
 *  Copies the internally stored GNSS data frame into the provided buffer.
 *
 *  @note       It is necessary to ensure that unread data are available in the
 *              internally stored data frame before calling this function.
 *
 *  @param      data_frame  Pointer to buffer into which GNSS data frame must be
 *                          copied.
 */

void gnss_read (gnss_data_frame_t * data_frame);

#endif
