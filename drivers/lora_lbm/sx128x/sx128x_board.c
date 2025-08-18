/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sx128x_board, CONFIG_LORA_BASICS_MODEM_DRIVERS_LOG_LEVEL);

#include "lora_lbm_transceiver.h"
#include "sx128x_hal_context.h"

#define SX128X_SPI_OPERATION (SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB)

/**
 * @brief Event pin callback handler.
 *
 * @param dev
 * @param cb
 * @param pins
 */
static void sx128x_board_event_callback(
	const struct device *dev, struct gpio_callback *cb, uint32_t pins, struct sx128x_hal_context_data_t *data)
{
	// This code expects to always use EDGE interrupt triggers (so no possible duplicate triggers)
	// NOTE: add interrupt pin to the sense-edge-mask to minimize power consumption!

	/* Call provided callback */
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD
	k_sem_give(&data->gpio_sem);
#elif CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD
	k_work_submit(&data->work);
#elif CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_DIRECT
	if (data->event_interrupt_cb) {
		data->event_interrupt_cb(data->sx128x_dev);
	}
#endif
}

#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
static void sx128x_board_dio1_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	struct sx128x_hal_context_data_t *data = CONTAINER_OF(cb, struct sx128x_hal_context_data_t, dio1_cb);
	return sx128x_board_event_callback(dev, cb, pins, data);
}
static void sx128x_board_dio2_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	struct sx128x_hal_context_data_t *data = CONTAINER_OF(cb, struct sx128x_hal_context_data_t, dio2_cb);
	return sx128x_board_event_callback(dev, cb, pins, data);
}
static void sx128x_board_dio3_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	struct sx128x_hal_context_data_t *data = CONTAINER_OF(cb, struct sx128x_hal_context_data_t, dio3_cb);
	return sx128x_board_event_callback(dev, cb, pins, data);
}
#endif

#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD
static void sx128x_thread(struct sx128x_hal_context_data_t *data)
{
	while (1) {
		k_sem_take(&data->gpio_sem, K_FOREVER);
		if (data->event_interrupt_cb) {
			data->event_interrupt_cb(data->sx128x_dev);
		}
	}
}
#endif /* CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD */

#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD
static void sx128x_work_cb(struct k_work *work)
{
	struct sx128x_hal_context_data_t *data =
		CONTAINER_OF(work, struct sx128x_hal_context_data_t, work);
	if (data->event_interrupt_cb) {
		data->event_interrupt_cb(data->sx128x_dev);
	}
}
#endif /* CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD */

void lora_transceiver_board_attach_interrupt(const struct device *dev, event_cb_t cb)
{
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
	struct sx128x_hal_context_data_t *data = dev->data;
	data->event_interrupt_cb = cb;
#else
	LOG_ERR("Event trigger not supported!");
#endif // CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
}

void lora_transceiver_board_enable_interrupt(const struct device *dev)
{
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
	const struct sx128x_hal_context_cfg_t *config = dev->config;
	if (config->dio1.port) {
		gpio_pin_interrupt_configure_dt(&config->dio1, GPIO_INT_EDGE_TO_ACTIVE);
	}
	if (config->dio2.port) {
		gpio_pin_interrupt_configure_dt(&config->dio2, GPIO_INT_EDGE_TO_ACTIVE);
	}
	if (config->dio3.port) {
		gpio_pin_interrupt_configure_dt(&config->dio3, GPIO_INT_EDGE_TO_ACTIVE);
	}
#else
	LOG_ERR("Event trigger not supported!");
#endif // CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
}

void lora_transceiver_board_disable_interrupt(const struct device *dev)
{
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
	const struct sx128x_hal_context_cfg_t *config = dev->config;
	if (config->dio1.port) {
		gpio_pin_interrupt_configure_dt(&config->dio1, GPIO_INT_DISABLE);
	}
	if (config->dio2.port) {
		gpio_pin_interrupt_configure_dt(&config->dio2, GPIO_INT_DISABLE);
	}
	if (config->dio3.port) {
		gpio_pin_interrupt_configure_dt(&config->dio3, GPIO_INT_DISABLE);
	}
#else
	LOG_ERR("Event trigger not supported!");
#endif // CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER
}

uint32_t lora_transceiver_get_tcxo_startup_delay_ms(const struct device *dev)
{
	//const struct sx126x_hal_context_cfg_t *config = dev->config;
	//return config->tcxo_cfg.wakeup_time_ms;

	// EvaTODO: do we need this, since we don't have TCXO support for SX128X?
	return 0;
}

static int sx128x_init(const struct device *dev)
{
	const struct sx128x_hal_context_cfg_t *config = dev->config;
	struct sx128x_hal_context_data_t *data = dev->data;
	int ret;

	if (!device_is_ready(config->spi.bus)) {
		LOG_ERR("Could not find SPI device");
		return -EINVAL;
	}

	// Reset pin
	ret = gpio_pin_configure_dt(&config->reset, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Could not configure reset gpio");
		return ret;
	}

	// Busy pin
	ret = gpio_pin_configure_dt(&config->busy, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure busy gpio");
		return ret;
	}

	// DIO1 event pin
	if (config->dio1.port) {
		ret = gpio_pin_configure_dt(&config->dio1, GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Could not configure DIO1 event gpio");
			return ret;
		}
	}
	// DIO2 event pin
	if (config->dio2.port) {
		ret = gpio_pin_configure_dt(&config->dio2, GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Could not configure DIO2 event gpio");
			return ret;
		}
	}
	// DIO3 event pin
	if (config->dio3.port) {
		ret = gpio_pin_configure_dt(&config->dio3, GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Could not configure DIO3 event gpio");
			return ret;
		}
	}

	data->sx128x_dev = dev;
	data->radio_status = RADIO_AWAKE;
	data->tx_offset = config->tx_offset;

	// Event pin trigger config
#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER

#ifdef CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_GLOBAL_THREAD
	data->work.handler = sx128x_work_cb;
#elif CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD
	k_sem_init(&data->trig_sem, 0, K_SEM_MAX_LIMIT);
	k_thread_create(&data->thread, data->thread_stack,
		CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_THREAD_STACK_SIZE,
		(k_thread_entry_t)sx128x_thread, data, NULL, NULL,
		K_PRIO_COOP(CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_THREAD_PRIORITY), 0, K_NO_WAIT);
#endif // CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER_OWN_THREAD

	if (config->dio1.port) {
		// Init callback
		gpio_init_callback(&data->dio1_cb, sx128x_board_dio1_callback, BIT(config->dio1.pin));
		// Add callback
		if (gpio_add_callback(config->dio1.port, &data->dio1_cb)) {
			LOG_ERR("Could not set dio1 pin callback");
			return -EIO;
		}
	}
	if (config->dio2.port) {
		// Init callback
		gpio_init_callback(&data->dio2_cb, sx128x_board_dio2_callback, BIT(config->dio2.pin));
		// Add callback
		if (gpio_add_callback(config->dio2.port, &data->dio2_cb)) {
			LOG_ERR("Could not set dio2 pin callback");
			return -EIO;
		}
	}
	if (config->dio3.port) {
		// Init callback
		gpio_init_callback(&data->dio3_cb, sx128x_board_dio3_callback, BIT(config->dio3.pin));
		// Add callback
		if (gpio_add_callback(config->dio3.port, &data->dio3_cb)) {
			LOG_ERR("Could not set dio3 pin callback");
			return -EIO;
		}
	}
#endif // CONFIG_LORA_BASICS_MODEM_DRIVERS_EVENT_TRIGGER

	return ret;
}

/*
 * Device creation macro.
 */

#define CONFIGURE_GPIO_IF_IN_DT(node_id, name, dt_prop)                       \
	COND_CODE_1(DT_NODE_HAS_PROP(node_id, dt_prop),                           \
		(.name = GPIO_DT_SPEC_GET(node_id, dt_prop),),                        \
		())

#define SX128X_CONFIG(node_id)                                                \
	{                                                                         \
		.spi = SPI_DT_SPEC_GET(node_id, SX128X_SPI_OPERATION, 0),             \
		.reset = GPIO_DT_SPEC_GET(node_id, reset_gpios),                      \
		.busy = GPIO_DT_SPEC_GET(node_id, busy_gpios),                        \
		CONFIGURE_GPIO_IF_IN_DT(node_id, dio1, dio1_gpios)                    \
		CONFIGURE_GPIO_IF_IN_DT(node_id, dio2, dio2_gpios)                    \
		CONFIGURE_GPIO_IF_IN_DT(node_id, dio3, dio3_gpios)                    \
		.reg_mode = DT_PROP(node_id, reg_mode),                               \
		.tx_offset = DT_PROP_OR(node_id, tx_power_offset, 0),                       \
	}

#define SX128X_DEVICE_INIT(node_id)                                           \
	DEVICE_DT_DEFINE(node_id, sx128x_init, PM_DEVICE_DT_GET(node_id),         \
			&sx128x_data_##node_id, &sx128x_config_##node_id,                 \
			POST_KERNEL, CONFIG_LORA_BASICS_MODEM_DRIVERS_INIT_PRIORITY, NULL);

#define SX128X_DEFINE(node_id)                                                                    \
	static struct sx128x_hal_context_data_t sx128x_data_##node_id;                                \
	static const struct sx128x_hal_context_cfg_t sx128x_config_##node_id = SX128X_CONFIG(node_id);   \
	SX128X_DEVICE_INIT(node_id)

DT_FOREACH_STATUS_OKAY(semtech_sx1280_new, SX128X_DEFINE)
DT_FOREACH_STATUS_OKAY(semtech_sx1281_new, SX128X_DEFINE)
