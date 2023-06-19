/** @defgroup   dummy Dummy
 *
 *  @brief      Dummy data frame generation.
 *
 *  This module generates dummy data frames. These data frames are created
 *  simply for the purpose of uploading, and do not contain any actual useful
 *  information themselves. This module internally stores a dummy data frame
 *  which can be read by calling dummy_read(). This data frame is never altered
 *  at any point of time.
 */

#ifndef __DUMMY_H__
#define __DUMMY_H__

#include "data.h"

/** @ingroup    dummy
 *
 *  @brief      Read dummy data frame.
 *
 *  Copies the internally stored dummy data frame into the provided buffer.
 *
 *  @param      data_frame  Pointer to buffer into which dummy data frame must
 *                          be copied.
 */

void dummy_read (dummy_data_frame_t * data_frame);

#endif
