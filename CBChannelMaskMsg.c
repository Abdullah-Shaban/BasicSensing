/**
 * This file is automatically generated by mig. DO NOT EDIT THIS FILE.
 * This file implements the functions for encoding and decoding the
 * 'cb_channelmask_msg' message type. See CBChannelMaskMsg.h for more details.
 */

#include <message.h>
#include "CBChannelMaskMsg.h"

uint16_t cb_channelmask_msg_data_get(tmsg_t *msg)
{
  return tmsg_read_ube(msg, 0, 16);
}

void cb_channelmask_msg_data_set(tmsg_t *msg, uint16_t value)
{
  tmsg_write_ube(msg, 0, 16, value);
}

