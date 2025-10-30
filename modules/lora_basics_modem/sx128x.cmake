# Copyright (c) 2024 Semtech Corporation - Félix Piédallu
# SPDX-License-Identifier: Apache-2.0

# Used by LBM
zephyr_compile_definitions(SX128X)
zephyr_compile_definitions(SX128X_DISABLE_WARNINGS)
zephyr_compile_definitions_ifdef(CONFIG_DT_HAS_SEMTECH_SX1280_NEW_ENABLED SX1280)
zephyr_compile_definitions_ifdef(CONFIG_DT_HAS_SEMTECH_SX1281_NEW_ENABLED SX1281)

set(LBM_SX128X_LIB_DIR ${LBM_RADIO_DRIVERS_DIR}/sx128x_driver/src)
zephyr_include_directories(${LBM_SX128X_LIB_DIR})

#-----------------------------------------------------------------------------
# Radio specific sources
#-----------------------------------------------------------------------------

zephyr_library_sources(
  ${LBM_SX128X_LIB_DIR}/sx128x.c
)

if(CONFIG_LORA_BASICS_MODEM_DRIVERS_RAL_RALF)
  zephyr_library_sources(
    ${LBM_SMTC_MODEM_CORE_DIR}/smtc_ral/src/ral_sx128x.c
    ${LBM_SMTC_MODEM_CORE_DIR}/smtc_ralf/src/ralf_sx128x.c
  )
endif()

# FIXME: why this specification? - NOTE: Probably because there was an error in regions.cmake file that is now fixed?
# LR1MAC_C_SOURCES += \
#   smtc_modem_core/lr1mac/src/smtc_real/src/region_ww_2g4.c
