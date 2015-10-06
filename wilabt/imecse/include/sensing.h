/*----------------------------------------------------------------------------
 * sensing.h
 *
 * Interface of the sensing platform
 *
 * Copyright (c) 2011 imec, Mattias Desmet
 * ---------------------------------------------------------------------------
 */

#ifndef SENSING_H
#define SENSING_H

#include <stdarg.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct se_s *se_t;

typedef enum {
    FFT_SWEEP = 0,
    WLAN_G = 1,
    WLAN_A = 2,
    BLUETOOTH = 3,
    ZIGBEE = 4,
    LTE = 5,
    DVB_T = 6,
    DVB_POWER = 7,
    ISM_POWER_DETECT = 8,
    TRANSMIT = 97,
    ADC_LOG1 = 98,
    ADC_LOG2 = 99,
    STDBY = 100
    } se_mode_t;

struct se_config_s {
    se_mode_t se_mode;
    int16_t fe_gain;
    uint32_t first_channel;
    uint32_t last_channel;
    uint32_t bandwidth;
    uint16_t fft_points;
    uint16_t dvb_nr_carriers;
    float dvb_guard_interval;
    float threshold_power;
};

se_t se_open(int spider, int frontend);
int se_init(se_t se_h, struct se_config_s *se_config);
void se_close(se_t se_h);

int se_check_config(se_t se_h, struct se_config_s se_config);
int se_configure(se_t se_h, struct se_config_s se_config, uint16_t mode);

int se_start_measurement(se_t se_h);
int se_stop_measurement(se_t se_h);

int se_get_result(se_t se_h, float *destination, ...);
int se_location(se_t se_h, float *current_loc);
int se_get_status(se_t se_h);

#ifdef __cplusplus
}
#endif

#endif /* SENSING_H */
