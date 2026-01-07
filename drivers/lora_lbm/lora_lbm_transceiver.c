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
#ifdef CONFIG_SEMTECH_LR11XX
		case RADIO_HAL_CONTEXT_LR11XX: {
			lr11xx_attach_interrupt(dev, cb);
			break;
		}
#endif /* CONFIG_SEMTECH_LR11XX */
#ifdef CONFIG_SEMTECH_SX126X
		case RADIO_HAL_CONTEXT_SX126X: {
			sx126x_attach_interrupt(dev, cb);
			break;
		}
#endif /* CONFIG_SEMTECH_SX126X */
#ifdef CONFIG_SEMTECH_SX128X
		case RADIO_HAL_CONTEXT_SX128X: {
			sx128x_attach_interrupt(dev, cb);
			break;
		}
#endif /* CONFIG_SEMTECH_SX128X */
		default:
			break;
	}
}

void lora_transceiver_board_enable_interrupt(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
#ifdef CONFIG_SEMTECH_LR11XX
		case RADIO_HAL_CONTEXT_LR11XX: {
			lr11xx_enable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_LR11XX */
#ifdef CONFIG_SEMTECH_SX126X
	case RADIO_HAL_CONTEXT_SX126X: {
			sx126x_enable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_SX126X */
#ifdef CONFIG_SEMTECH_SX128X
		case RADIO_HAL_CONTEXT_SX128X: {
			sx128x_enable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_SX128X */
		default:
			break;
	}
}

void lora_transceiver_board_disable_interrupt(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
#ifdef CONFIG_SEMTECH_LR11XX
		case RADIO_HAL_CONTEXT_LR11XX: {
			lr11xx_disable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_LR11XX */
#ifdef CONFIG_SEMTECH_SX126X
		case RADIO_HAL_CONTEXT_SX126X: {
			sx126x_disable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_SX126X */
#ifdef CONFIG_SEMTECH_SX128X
		case RADIO_HAL_CONTEXT_SX128X: {
			sx128x_disable_interrupt(dev);
			break;
		}
#endif /* CONFIG_SEMTECH_SX128X */
		default:
			break;
	}
}

uint32_t lora_transceiver_get_tcxo_startup_delay_ms(const struct device *dev)
{
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);

	switch (*context_type) {
#ifdef CONFIG_SEMTECH_LR11XX
		case RADIO_HAL_CONTEXT_LR11XX: {
			return lr11xx_get_tcxo_startup_delay_ms(dev);
		}
#endif /* CONFIG_SEMTECH_LR11XX */
#ifdef CONFIG_SEMTECH_SX126X
		case RADIO_HAL_CONTEXT_SX126X: {
			return sx126x_get_tcxo_startup_delay_ms(dev);
		}
#endif /* CONFIG_SEMTECH_SX126X */
#ifdef CONFIG_SEMTECH_SX128X
		case RADIO_HAL_CONTEXT_SX128X: {
			return sx128x_get_tcxo_startup_delay_ms(dev);
		}
#endif /* CONFIG_SEMTECH_SX128X */
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
#ifdef CONFIG_SEMTECH_LR11XX
		case RADIO_HAL_CONTEXT_LR11XX: {
			return lr11xx_get_model(dev);
		}
#endif /* CONFIG_SEMTECH_LR11XX */
		default:
			break;
	}
	return -1;
}

void lora_transceiver_set_front_end_module_cbs(const struct device *dev, struct front_end_module_cbs_t fem_cbs)
{
#ifdef CONFIG_LORA_BASIC_MODEM_EXTERNAL_FRONT_END_MODULE
	const radio_hal_context_type_t *context_type =
		((const radio_hal_context_type_t *)dev->config);
	switch (*context_type) {
#ifdef CONFIG_SEMTECH_SX128X
		case RADIO_HAL_CONTEXT_SX128X: {
			sx128x_set_front_end_module_cbs(dev, fem_cbs);
			break;
		}
#endif /* CONFIG_SEMTECH_SX128X */
		/* EvaTODO: add support for other board types */
		default:
			break;
		}
#endif /* CONFIG_LORA_BASIC_MODEM_EXTERNAL_FRONT_END_MODULE */
}
