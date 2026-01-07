/*
 * radio_utilities.c
 * Utility functions for radio operations in LBM Zephyr LoRa driver.
 *
 * Copyright (c) 2025 IRNAS
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "radio_utilities.h"
#include "radio_hal_context.h"

#include <zephyr/kernel.h>

#ifdef CONFIG_SEMTECH_LR11XX
#include "lr11xx_hal_context.h"
#endif

#ifdef CONFIG_SEMTECH_SX126X
#include "sx126x_hal_context.h"
#endif

#ifdef CONFIG_SEMTECH_SX128X
#include "sx128x_hal_context.h"
#endif

uint8_t radio_utilities_get_tx_power_offset(const void *context)
{
	const struct device *dev = (const struct device *)context;

	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			struct lr11xx_hal_context_data_t *data = dev->data;
			return data->tx_offset;
#else
			return 0;
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			struct sx126x_hal_context_data_t *data = dev->data;
			return data->tx_offset;
#else
			return 0;
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			struct sx128x_hal_context_data_t *data = dev->data;
			return data->tx_offset;
#else
			return 0;
#endif
		}
		default:
			break;
	}
	return 0;
}

void radio_utilities_set_tx_power_offset(const void *context, uint8_t tx_pwr_offset_db)
{
	const struct device *dev = (const struct device *)context;

	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			struct lr11xx_hal_context_data_t *data = dev->data;
			data->tx_offset = tx_pwr_offset_db;
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			struct sx126x_hal_context_data_t *data = dev->data;
			data->tx_offset = tx_pwr_offset_db;
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			struct sx128x_hal_context_data_t *data = dev->data;
			data->tx_offset = tx_pwr_offset_db;
#endif
		}
		default:
			break;
	}
}
