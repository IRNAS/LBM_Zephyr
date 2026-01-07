/**
 * @file radio_hal_context.h
 * @brief Radio HAL context definitions.
 *
 * Copyright (c) 2025 IRNAS d.o.o.
 */

#ifndef RADIO_HAL_CONTEXT_H
#define RADIO_HAL_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	RADIO_HAL_CONTEXT_LR11XX,
	RADIO_HAL_CONTEXT_SX126X,
	RADIO_HAL_CONTEXT_SX128X
} radio_hal_context_type_t;

#ifdef __cplusplus
}
#endif

#endif // RADIO_HAL_CONTEXT_H
