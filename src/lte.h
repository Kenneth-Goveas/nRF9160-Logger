/** @defgroup   lte LTE
 *
 *  @brief      LTE connectivity.
 *
 *  This module controls the modem LTE interface. The interface can be activated
 *  by calling lte_init(). While active, the interface continuously tries to
 *  obtain an LTE connection. If connection is lost, the interface attempts to
 *  re-establish it. The availability of an LTE connection can be checked with
 *  lte_conn_avail(). Every time a network notification is received, an
 *  internally stored LTE data frame is updated with the newly received data.
 *  The availability of such data can be checked with lte_data_avail(). The LTE
 *  data frame can be read by calling lte_read(). When not required, the LTE
 *  interface can be deactivated by calling lte_deinit().
 */

#ifndef __LTE_H__
#define __LTE_H__

#include <stdbool.h>

#include "data.h"

/** @ingroup    lte
 *
 *  @brief      Initialize LTE interface.
 *
 *  Powers on and initializes the modem LTE interface. The GNSS interface
 *  remains powered off.
 *
 *  @note       This function must not be called while the GNSS interface is
 *              active.
 *
 *  @retval     0   Success.
 *  @retval     -1  Failure. Interface is powered back off in this case.
 */

int lte_init (void);

/** @ingroup    lte
 *
 *  @brief      Deinitialize LTE interface.
 *
 *  Deinitializes and powers off the modem LTE interface.
 */

void lte_deinit (void);

/** @ingroup    lte
 *
 *  @brief      Check for LTE connection.
 *
 *  Checks if an LTE network connection is established.
 *
 *  @retval     true    Connected to LTE network.
 *  @retval     false   Not connected to LTE network.
 */

bool lte_conn_avail (void);

/** @ingroup    lte
 *
 *  @brief      Wait for LTE connection.
 *
 *  Waits until an LTE network connection is established. This call fails if the
 *  configured LTE connection timeout expires.
 *
 *  @retval     0   Success.
 *  @retval     -1  Failure.
 */

int lte_wait_conn_avail (void);

/** @ingroup    lte
 *
 *  @brief      Check for unread LTE data.
 *
 *  Checks if the internally stored LTE data frame contains unread data.
 *
 *  @retval     true    LTE data available.
 *  @retval     false   LTE data unavailable.
 */

bool lte_data_avail (void);

/** @ingroup    lte
 *
 *  @brief      Wait for unread LTE data.
 *
 *  Waits until unread data become available in the internally stored LTE data
 *  frame. This call fails if the configured LTE data update timeout expires.
 *
 *  @retval     0   Success.
 *  @retval     -1  Failure.
 */

int lte_wait_data_avail (void);

/** @ingroup    lte
 *
 *  @brief      Read LTE data frame.
 *
 *  Copies the internally stored LTE data frame into the provided buffer.
 *
 *  @note       It is necessary to ensure that unread data are available in the
 *              internally stored data frame before calling this function.
 *
 *  @param      data_frame  Pointer to buffer into which LTE data frame must be
 *                          copied.
 */

void lte_read (lte_data_frame_t * data_frame);

#endif
