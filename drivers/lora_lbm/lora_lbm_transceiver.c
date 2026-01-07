/*
 * lora_lbm_transceiver.c
 * LoRa LBM Transceiver Driver Implementation
 *
 * Copyright (c) 2025 IRNAS
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lora_lbm_transceiver.h"
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

void lora_transceiver_board_attach_interrupt(const struct device *dev, event_cb_t cb)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			lr11xx_attach_interrupt(dev, cb);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			sx126x_attach_interrupt(dev, cb);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			sx128x_attach_interrupt(dev, cb);
			break;
#endif
		}
		default:
			break;
	}
}

void lora_transceiver_board_enable_interrupt(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			lr11xx_enable_interrupt(dev);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			sx126x_enable_interrupt(dev);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			sx128x_enable_interrupt(dev);
			break;
#endif
		}
		default:
			break;
	}
}

void lora_transceiver_board_disable_interrupt(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			lr11xx_disable_interrupt(dev);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			sx126x_disable_interrupt(dev);
			break;
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			sx128x_disable_interrupt(dev);
			break;
#endif
		}
		default:
			break;
	}
}

uint32_t lora_transceiver_get_tcxo_startup_delay_ms(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			return lr11xx_get_tcxo_startup_delay_ms(dev);
#endif
		}
		case RADIO_HAL_CONTEXT_SX126X: {
#ifdef CONFIG_SEMTECH_SX126X
			return sx126x_get_tcxo_startup_delay_ms(dev);
#endif
		}
		case RADIO_HAL_CONTEXT_SX128X: {
#ifdef CONFIG_SEMTECH_SX128X
			return sx128x_get_tcxo_startup_delay_ms(dev);
#endif
		}
		default:
			break;
	}

	return 0;
}

int32_t lora_transceiver_get_model(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
		case RADIO_HAL_CONTEXT_LR11XX: {
#ifdef CONFIG_SEMTECH_LR11XX
			return lr11xx_get_model(dev);
#endif
		}
		default:
			break;
	}
	return -1;
}
