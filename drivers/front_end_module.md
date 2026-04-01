# Front-End-Module support

We add Front-end-module support to the lora radio drivers, currently implemented only for `sx128X` radio. Implementation tries to introduce minimal changes to the radio drivers and `lora-basic-modem` lib.

## Init

To enable front-end-module support, user needs to set `CONFIG_LORA_BASIC_MODEM_EXTERNAL_FRONT_END_MODULE` in the project settings file. Then after radio driver is initialized, user needs to configure front-end-module via `lora_transceiver_configure_front_end_module()`, providing configuration

```bash
struct front_end_module_cfg_t {
	struct front_end_module_cbs_t cbs;
	uint8_t tx_pwr_threshold_dbm; /* Threshold above which the FEM should be set to TX mode */
	uint8_t tx_max_pwr_dbm;       /* Maximum output power of the FEM in dB */
	uint8_t gain_dbm;             /* Gain provided by the FEM in dB */
    bool active;                  /* Whether to use the FEM */
};
```

where `tx_pwr_threshold_dbm` defines dbm threshold, above which TX mode is selected over BYPASS, `tx_max_pwr_dbm` defines new max output power and `gain_dbm` is defined gain of the front-end-module. `active` flag determines if module should be use or not. All callbacks

```bash
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
```

must be provided.

## `lora_basic_modem` compatibility

To make front-end module compatible with operation of the `lora_basic_modem`, before TX power is set via `sx128x_set_tx_params` call, `ral_sx128x_bsp_get_tx_cfg` must be called to check on power settings and make modifications based on the set power. This is already done in `lora_basic_modem`. If user is calling radio functions directly, it is his responsibility to call that.

`ral_sx128x_bsp_get_tx_cfg` will check if there is support for front-end module and if it is configured. If not, only power limitations and board offset will be taken into the account when setting configured power, otherwise function will determine mode: bypass or TX and shift power setting for module gain in TX mode is selected.

Before after setting TX parameters and before entering TX or RX mode, lbm lib will call `ral_sx128x_bsp_set_front_end_tx` or `ral_sx128x_bsp_set_front_end_rx`. `ral_sx128x_bsp_set_front_end_tx` will select correct mode (TX or BYPASS), provided lbm lib did a call to `ral_sx128x_bsp_get_tx_cfg` beforehand. Support is added to lbm lib.
When TX or RX mode is done, `ral_sx128x_bsp_set_front_end_off` is called. This is implemented in the lbm lib set sleep and set standby functions.

## Direct radio usage

If user is calling radio functions directly, it is his responsibility to set correct front-end modes.

### TX mode

Call `ral_sx128x_bsp_get_tx_cfg` with desired parameters and use provided `tx_cfg_output_params.chip_output_pwr_in_dbm_configured` power to set power with `sx128x_set_tx_params` function.

Turn on TX/BAYPASS mode based on desired power setting (`tx_cfg_input_params.system_output_pwr_in_dbm`). Set chip in TX mode with `sx128x_set_tx`.

On TX done event, turn off front end module.

### RX mode

Turn on RX mode on front-end module before setting chip in RX mode with `sx128x_set_rx`. On RX done event or timeout, turn off front end module.
