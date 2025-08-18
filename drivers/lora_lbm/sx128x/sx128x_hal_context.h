/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SX128X_HAL_CONTEXT_H
#define SX128X_HAL_CONTEXT_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <ral_sx128x_bsp.h>

#include <sx128x.h>

#ifdef __cplusplus
extern "C" {
#endif


struct sx128x_hal_context_cfg_t {
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
	RADIO_SLEEP,
	RADIO_AWAKE
} radio_sleep_status_t;

/**
 * @brief Callback upon firing event trigger
 *
 */
typedef void (*event_cb_t)(const struct device *dev);

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
	radio_sleep_status_t radio_status;
	uint8_t tx_offset; /* Board TX power offset at reset */
};


#ifdef __cplusplus
}
#endif

#endif /* SX128X_HAL_CONTEXT_H */
