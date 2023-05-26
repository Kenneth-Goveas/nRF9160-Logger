#ifndef __LTE_H__
#define __LTE_H__

#include <stdbool.h>

#include "data.h"

int lte_init (void);
void lte_deinit (void);
bool lte_conn_avail (void);
int lte_wait_conn_avail (void);
bool lte_data_avail (void);
int lte_wait_data_avail (void);
void lte_read (lte_data_frame_t * data_frame);

#endif
