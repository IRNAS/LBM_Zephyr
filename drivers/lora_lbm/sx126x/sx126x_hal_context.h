/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SX126X_HAL_CONTEXT_H
#define SX126X_HAL_CONTEXT_H

#include "lora_lbm_transceiver.h"

#include <radio_hal_context.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <ral_sx126x_bsp.h>

#include <sx126x.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sx126x_hal_context_tcxo_cfg_t {
	ral_xosc_cfg_t xosc_cfg;
	sx126x_tcxo_ctrl_voltages_t voltage;
	uint32_t wakeup_time_ms;
};

typedef struct sx126x_pa_pwr_cfg_s
{
	int8_t  power;
	uint8_t pa_duty_cycle;
	uint8_t pa_hp_sel;
} sx126x_pa_pwr_cfg_t;


struct sx126x_hal_context_cfg_t {
	radio_hal_context_type_t type; /* Context type identifier */

	struct spi_dt_spec spi; /* spi peripheral */

	struct gpio_dt_spec reset;  /* reset pin */
	struct gpio_dt_spec busy;   /* busy pin */

	struct gpio_dt_spec dio1;   /* DIO1 pin */
	struct gpio_dt_spec dio2;   /* DIO2 pin */
	struct gpio_dt_spec dio3;   /* DIO3 pin */

	bool dio2_as_rf_switch;
	struct sx126x_hal_context_tcxo_cfg_t tcxo_cfg; /* TCXO config, says if dio3-tcxo */
	uint8_t capa_xta; /* set to 0xFF if not configured*/
	uint8_t capa_xtb; /* set to 0xFF if not configured*/

	sx126x_reg_mod_t reg_mode;
	bool rx_boosted; /* RXBoosted option */

	uint8_t tx_offset; /* Board TX power offset */
};


// This type holds the current sleep status of the radio
typedef enum {
	SX126X_SLEEP,
	SX126X_AWAKE
} sx126x_sleep_status_t;


struct sx126x_hal_context_data_t {
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
	const struct device *sx126x_dev;
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
	sx126x_sleep_status_t radio_status;
	uint8_t tx_offset; /* Board TX power offset at reset */
};


// FIXME: sx126x_standby_cfgs_e, sx126x_reg_mods_e, sx126x_tcxo_ctrl_voltages_e

// LoRa LBM Transceiver interface implementation
void sx126x_attach_interrupt(const struct device *dev, event_cb_t cb);
void sx126x_enable_interrupt(const struct device *dev);
void sx126x_disable_interrupt(const struct device *dev);
uint32_t sx126x_get_tcxo_startup_delay_ms(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif /* SX126X_HAL_CONTEXT_H */
