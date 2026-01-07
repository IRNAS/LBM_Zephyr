/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LORA_LBM_TRANSCEIVER_H
#define LORA_LBM_TRANSCEIVER_H

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback upon firing event trigger
 *
 */
typedef void (*event_cb_t)(const struct device *dev);

/**
 * @brief Front-end module callbacks structure
 *
 */
struct front_end_module_cbs_t {
	/**
	 * @brief Turn off the front-end module.
	 *
	 */
	void (*off)(void);
	/**
	 * @brief Set the front-end module in bypass mode.
	 *
	 */
	void (*bypass)(void);
	/**
	 * @brief Set the front-end module in receive mode.
	 *
	 */
	void (*rx)(void);
	/**
	 * @brief Set the front-end module in transmit mode.
	 *
	 */
	void (*tx)(void);
};

/**
 * @brief Attach interrupt cb to event pin.
 *
 * @param dev context
 * @param cb cb function
 */
void lora_transceiver_board_attach_interrupt(const struct device *dev, event_cb_t cb);

/**
 * @brief Enable interrupt on event pin.
 *
 * @param dev context
 */
void lora_transceiver_board_enable_interrupt(const struct device *dev);

/**
 * @brief Disable interrupt on event pin.
 *
 * @param dev context
 */
void lora_transceiver_board_disable_interrupt(const struct device *dev);

/**
 * @brief Helper to get the tcxo startup delay for any model of transceiver
 *
 * @param dev context
 */
uint32_t lora_transceiver_get_tcxo_startup_delay_ms(const struct device *dev);

/**
 * @brief Returns lr11xx_system_version_type_t or -1
 *
 * @param dev context
 */

int32_t lora_transceiver_get_model(const struct device *dev);

/**
 * @brief Set front-end module callbacks
 *
 * @param[in] dev context
 * @param[in] fem_cbs front-end module callbacks
 */
void lora_transceiver_set_front_end_module_cbs(const struct device *dev,
	struct front_end_module_cbs_t fem_cbs);

#ifdef __cplusplus
}
#endif

#endif // LORA_LBM_TRANSCEIVER_H
