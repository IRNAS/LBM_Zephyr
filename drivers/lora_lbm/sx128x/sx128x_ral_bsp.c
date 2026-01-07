/*
 * Copyright (c) 2024 Semtech Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include "ral_sx128x_bsp.h"
#include "radio_utilities.h"
#include "sx128x.h"
#include "sx128x_hal_context.h"


void ral_sx128x_bsp_get_reg_mode(const void* context, sx128x_reg_mod_t* reg_mode)
{
	const struct device *dev = context;
	const struct sx128x_hal_context_cfg_t *config = dev->config;
	*reg_mode = config->reg_mode;
}

void ral_sx128x_bsp_get_tx_cfg(const void* context,
	const ral_sx128x_bsp_tx_cfg_input_params_t* input_params,
	ral_sx128x_bsp_tx_cfg_output_params_t* output_params)
{
	// get board tx power offset
	int8_t board_tx_pwr_offset_db = radio_utilities_get_tx_power_offset(context);

	int16_t power = input_params->system_output_pwr_in_dbm + board_tx_pwr_offset_db;

	if( power > 13 )
	{
		output_params->chip_output_pwr_in_dbm_configured = 13;
		output_params->chip_output_pwr_in_dbm_expected   = 13;
	}
	else if( power < -18 )
	{
		output_params->chip_output_pwr_in_dbm_configured = -18;
		output_params->chip_output_pwr_in_dbm_expected   = -18;
	}
	else
	{
		output_params->chip_output_pwr_in_dbm_configured = ( int8_t ) power;
		output_params->chip_output_pwr_in_dbm_expected   = ( int8_t ) power;
	}

	output_params->pa_ramp_time  = SX128X_RAMP_10_US;
}

void ral_sx128x_bsp_get_lora_cad_det_peak(const void *context, ral_lora_sf_t sf, ral_lora_bw_t bw,
                                          ral_lora_cad_symbs_t nb_symbol, uint8_t* in_out_cad_det_peak)
{
	// Function used to fine tune the cad detection peak, update if needed
}

#define SX128X_MIN_OUTPUT_POWER -18
#define SX128X_MAX_OUTPUT_POWER 13

#define SX128X_CONVERT_TABLE_INDEX_OFFSET 18


#define SX128X_LORA_RX_CONSUMPTION_BW_200_DCDC 5500
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_200_DCDC 6200

#define SX128X_LORA_RX_CONSUMPTION_BW_400_DCDC 6000
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_400_DCDC 6700

#define SX128X_LORA_RX_CONSUMPTION_BW_800_DCDC 7000
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_800_DCDC 7700

#define SX128X_LORA_RX_CONSUMPTION_BW_1600_DCDC 7500
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_1600_DCDC 8200

#define SX128X_LORA_RX_CONSUMPTION_BW_200_LDO 10800
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_200_LDO 12200

#define SX128X_LORA_RX_CONSUMPTION_BW_400_LDO 11800
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_400_LDO 13200

#define SX128X_LORA_RX_CONSUMPTION_BW_800_LDO 13700
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_800_LDO 15200

#define SX128X_LORA_RX_CONSUMPTION_BW_1600_LDO 14800
#define SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_1600_LDO 16300

static const uint32_t ral_sx128x_convert_tx_dbm_to_ua_reg_mode_dcdc[] = {
    6200,   // -18 dBm
    6300,   // -17 dBm
    6400,   // -16 dBm
    6500,   // -15 dBm
    6600,   // -14 dBm
    6700,   // -13 dBm
    6800,   // -12 dBm
    7000,   // -11 dBm
    7100,   // -10 dBm
    7300,   //  -9 dBm
    7400,   //  -8 dBm
    7700,   //  -7 dBm
    7900,   //  -6 dBm
    8100,   //  -5 dBm
    8500,   //  -4 dBm
    8800,   //  -3 dBm
    9200,   //  -2 dBm
    9700,   //  -1 dBm
    10100,  //   0 dBm
    10700,  //   1 dBm
    11300,  //   2 dBm
    12000,  //   3 dBm
    12700,  //   4 dBm
    13600,  //   5 dBm
    14500,  //   6 dBm
    15500,  //   7 dBm
    16800,  //   8 dBm
    17700,  //   9 dBm
    18600,  //  10 dBm
    20300,  //  11 dBm
    22000,  //  12 dBm
    24000,  //  13 dBm
};

static const uint32_t ral_sx128x_convert_tx_dbm_to_ua_reg_mode_ldo[] = {
    11800,  // -18 dBm
    12000,  // -17 dBm
    12200,  // -16 dBm
    12400,  // -15 dBm
    12600,  // -14 dBm
    12800,  // -13 dBm
    13000,  // -12 dBm
    13300,  // -11 dBm
    13500,  // -10 dBm
    14000,  //  -9 dBm
    14200,  //  -8 dBm
    14700,  //  -7 dBm
    15200,  //  -6 dBm
    15600,  //  -5 dBm
    16300,  //  -4 dBm
    17000,  //  -3 dBm
    17700,  //  -2 dBm
    18600,  //  -1 dBm
    19600,  //   0 dBm
    20700,  //   1 dBm
    21900,  //   2 dBm
    23200,  //   3 dBm
    24600,  //   4 dBm
    26300,  //   5 dBm
    28000,  //   6 dBm
    30000,  //   7 dBm
    32200,  //   8 dBm
    34500,  //   9 dBm
    36800,  //  10 dBm
    39200,  //  11 dBm
    41900,  //  12 dBm
    45500,  //  13 dBm
};


__weak ral_status_t ral_sx128x_bsp_get_instantaneous_tx_power_consumption(
	const void *context,
	ral_sx128x_bsp_tx_cfg_output_params_t tx_cfg_output_params,
	sx128x_reg_mod_t radio_reg_mode,
	uint32_t* pwr_consumption_in_ua )
{

    uint8_t index = 0;

    if( tx_cfg_output_params.chip_output_pwr_in_dbm_expected > SX128X_PWR_MAX )
    {
	index = SX128X_PWR_MAX + SX128X_CONVERT_TABLE_INDEX_OFFSET;
    }
    else if( tx_cfg_output_params.chip_output_pwr_in_dbm_expected < SX128X_PWR_MIN )
    {
	index = SX128X_PWR_MIN + SX128X_CONVERT_TABLE_INDEX_OFFSET;
    }
    else
    {
	index = tx_cfg_output_params.chip_output_pwr_in_dbm_expected + SX128X_CONVERT_TABLE_INDEX_OFFSET;
    }

    if( radio_reg_mode == SX128X_REG_MODE_DCDC )
    {
	*pwr_consumption_in_ua = ral_sx128x_convert_tx_dbm_to_ua_reg_mode_dcdc[index];
    }
    else if( radio_reg_mode == SX128X_REG_MODE_LDO )
    {
        *pwr_consumption_in_ua = ral_sx128x_convert_tx_dbm_to_ua_reg_mode_ldo[index];
    }
    else
    {
        return RAL_STATUS_UNKNOWN_VALUE;
    }

    return RAL_STATUS_OK;
}

__weak ral_status_t ral_sx128x_bsp_get_instantaneous_gfsk_rx_power_consumption(
	const void *context,
	sx128x_reg_mod_t radio_reg_mode,
	bool rx_boosted,
	uint32_t* pwr_consumption_in_ua)
{
    return RAL_STATUS_UNSUPPORTED_FEATURE;
}

__weak ral_status_t ral_sx128x_bsp_get_instantaneous_lora_rx_power_consumption(
	const void *context,
	sx128x_reg_mod_t reg_mode,
	ral_lora_bw_t bw,
	bool rx_boosted,
	uint32_t* pwr_consumption_in_ua)
{
    switch( reg_mode )
    {
    case SX128X_REG_MODE_DCDC:
    {
        switch( bw )
        {
        case RAL_LORA_BW_200_KHZ:
        {
            *pwr_consumption_in_ua = ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_200_DCDC
                                                    : SX128X_LORA_RX_CONSUMPTION_BW_200_DCDC;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_400_KHZ:
        {
            *pwr_consumption_in_ua = ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_400_DCDC
                                                    : SX128X_LORA_RX_CONSUMPTION_BW_400_DCDC;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_800_KHZ:
        {
            *pwr_consumption_in_ua = ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_800_DCDC
                                                    : SX128X_LORA_RX_CONSUMPTION_BW_800_DCDC;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_1600_KHZ:
        {
            *pwr_consumption_in_ua = ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_1600_DCDC
                                                    : SX128X_LORA_RX_CONSUMPTION_BW_1600_DCDC;
            return RAL_STATUS_OK;
        }
        default:
            return RAL_STATUS_UNKNOWN_VALUE;
        }
        break;
    }
    case SX128X_REG_MODE_LDO:
    {
        switch( bw )
        {
        case RAL_LORA_BW_200_KHZ:
        {
            *pwr_consumption_in_ua =
                ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_200_LDO : SX128X_LORA_RX_CONSUMPTION_BW_200_LDO;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_400_KHZ:
        {
            *pwr_consumption_in_ua =
                ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_400_LDO : SX128X_LORA_RX_CONSUMPTION_BW_400_LDO;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_800_KHZ:
        {
            *pwr_consumption_in_ua =
                ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_800_LDO : SX128X_LORA_RX_CONSUMPTION_BW_800_LDO;
            return RAL_STATUS_OK;
        }
        case RAL_LORA_BW_1600_KHZ:
        {
            *pwr_consumption_in_ua = ( rx_boosted ) ? SX128X_LORA_RX_BOOSTED_CONSUMPTION_BW_1600_LDO
                                                    : SX128X_LORA_RX_CONSUMPTION_BW_1600_LDO;
            return RAL_STATUS_OK;
        }
        default:
        {
            return RAL_STATUS_UNKNOWN_VALUE;
        }
        }
        break;
    }
    default:
    {
        return RAL_STATUS_UNKNOWN_VALUE;
    }
    }
}
