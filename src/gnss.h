#ifndef __GNSS_H__
#define __GNSS_H__

#include <stdbool.h>

#include "data.h"

int gnss_init (void);
void gnss_deinit (void);
bool gnss_data_avail (void);
int gnss_wait_data_avail (void);
void gnss_read (gnss_data_frame_t * data_frame);

#endif
