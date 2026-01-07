/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SX128X_HAL_CONTEXT_H
#define SX128X_HAL_CONTEXT_H

#include "lora_lbm_transceiver.h"
#include <radio_hal_context.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <ral_sx128x_bsp.h>

#include <sx128x.h>

#ifdef __cplusplus
extern "C" {
#endif


struct sx128x_hal_context_cfg_t {
	radio_hal_context_type_t type; /* Context type identifier */

	struct spi_dt_spec spi; /* spi peripheral */

	struct gpio_dt_spec reset;  /* reset pin */
	struct gpio_dt_spec busy;   /* busy pin */

	struct gpio_dt_spec dio1;   /* DIO1 pin */
	struct gpio_dt_spec dio2;   /* DIO2 pin */
	struct gpio_dt_spec dio3;   /* DIO3 pin */

	sx128x_reg_mod_t reg_mode;

	uint8_t tx_offset; /* Board TX power offset */
};


// This type holds the current sleep status of the radio
typedef enum {
	SX128X_SLEEP,
	SX128X_AWAKE
} sx128x_sleep_status_t;

struct sx128x_hal_context_data_t {
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
	const struct device *sx128x_dev;
	struct gpio_callback dio1_cb; /* event callback structure */
	struct gpio_callback dio2_cb; /* event callback structure */
	struct gpio_callback dio3_cb; /* event callback structure */
	event_cb_t event_interrupt_cb; /* event interrupt user provided callback */

#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD
	struct k_work work;
#endif /* CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD */
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD
	K_THREAD_STACK_MEMBER(thread_stack, CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_THREAD_STACK_SIZE);
	struct k_thread thread;
	struct k_sem trig_sem;
#endif /* CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD */
#endif /* CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER */
	sx128x_sleep_status_t radio_status;
	uint8_t tx_offset; /* Board TX power offset at reset */
};

// LoRa LBM Transceiver interface implementation
void sx128x_attach_interrupt(const struct device *dev, event_cb_t cb);
void sx128x_enable_interrupt(const struct device *dev);
void sx128x_disable_interrupt(const struct device *dev);
uint32_t sx128x_get_tcxo_startup_delay_ms(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif /* SX128X_HAL_CONTEXT_H */
