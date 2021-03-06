/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

#ifdef ADC_CONFIG
/**
 * @brief   Load the ADC configuration
 */
static const uint8_t adc_config[] = ADC_CONFIG;

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock;

static inline void prep(void)
{
    mutex_lock(&lock);
    NRF_ADC->POWER = 1;
    NRF_ADC->ENABLE = 1;
}

static inline void done(void)
{
    NRF_ADC->ENABLE = 0;
    NRF_ADC->POWER = 0;
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -1;
    }
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int val;

    /* check if resolution is valid */
    if (res > 2) {
        return -1;
    }

    /* prepare device */
    prep();

    /* set resolution, line, and use 1/3 input and ref voltage scaling */
    NRF_ADC->CONFIG = ((ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling << 5) |
                       (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << 2) |
                       (1 << (adc_config[line] + 8)) |
                       res);
    /* start conversion */
    NRF_ADC->TASKS_START = 1;
    /* wait for conversion to be complete */
    while (NRF_ADC->BUSY == 1) {}
    /* get result */
    val = (int)NRF_ADC->RESULT;

    /* free device */
    done();

    return val;
}

#else
typedef int dont_be_pedantic;
#endif /* ADC_CONFIG */
