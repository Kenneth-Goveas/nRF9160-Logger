#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "data.h"

LOG_MODULE_REGISTER(dummy, CONFIG_DUMMY_LOG_LEVEL);

static dummy_data_frame_t _dummy_data_frame = {
    .field1 = "First field of dummy data frame with 256 bytes",
    .field2 = "Second field of dummy data frame of length 256 bytes",
    .field3 = "Third field of dummy data frame containing 256 bytes",
    .field4 = "Fourth field of dummy data frame which contains 256 bytes"
};

void dummy_read (dummy_data_frame_t * data_frame) {
    memcpy(data_frame, &_dummy_data_frame, sizeof(_dummy_data_frame));
}
