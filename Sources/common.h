/*
 * Copyright 2016 - 2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "Cpu.h"
#include "bcc/bcc.h"

/*******************************************************************************
 * Definitions (depends on the utilized device/board/battery)
 ******************************************************************************/

/* Used BCC device */
#define MC33771     /* Define MC33771 if FRDM33771BSPIEVB or FRDM33771BTPLEVB is used. */
//#define MC33772   /* Define MC33772 if FRDM33772BSPIEVB or FRDM33772BTPLEVB is used. */

/* Used BCC board/communication. */
#define SPI         /* Define SPI if S32K144EVB + FRDM33771BSPIEVB/FRDM33772BSPIEVB is used. */
//#define TPL         /* Define TPL if S32K144EVB is interconnected with FRDM33664BEVB by wires according to UM11143. */
//#define TPL_TRANSLT /* Define TPL_TRANSLT if S32K144EVB is interconnected with FRDM33664BEVB by Translator board (X_TRANSLT_DEV). */

/* Battery type. */
#define BCC_DEMO_BATTERY_TYPE   BCC_BATT_T

/* R_SHUNT value in [uOhm] */
#define DEMO_RSHUNT             100000U

#if !defined(MC33771) && !defined(MC33772)
    #error "Select used BCC device by defining MC33771 or MC33772."
#endif

#if defined(MC33771) && defined(MC33772)
    #error "Select only one type of BCC device."
#endif

#if !defined(SPI) && !defined(TPL)  && !defined(TPL_TRANSLT)
    #error "Select used type of communication by defining SPI, TPL or TPL_TRANSLT."
#endif

#if defined(TPL) && defined(TPL_TRANSLT)
    #error "Select only one type of communication."
#endif
#if defined(SPI) && (defined(TPL) || defined(TPL_TRANSLT))
    #error "Select only one type of communication."
#endif

/*******************************************************************************
 * Initial register configuration
 ******************************************************************************/

/* Note: INIT register is initialized automatically by the BCC driver. */
/* Note: SYS_CFG_GLOBAL register contains only command GO2SLEEP (no initialization needed). */
/* Note: EEPROM_CTRL, FUSE_MIRROR_DATA and FUSE_MIRROR_CNTL registers are not initialized. */

#define BCC_CONF1_SYS_CFG1_VALUE ( \
    BCC_CYCLIC_TIMER_0_1S | \
    BCC_DIAG_TIMEOUT_1S | \
    BCC_I_MEAS_ENABLED | \
    BCC_CB_AUTO_PAUSE_ENABLED | \
    BCC_CB_DRV_DISABLED | \
    BCC_DIAG_MODE_DISABLED | \
    BCC_CB_MAN_PAUSE_DISABLED | \
    BCC_SW_RESET_DISABLED | \
    BCC_FAULT_WAVE_DISABLED | \
    BCC_WAVE_DC_500US \
)

/* Initial value of SYS_CFG2 register. */
#define BCC_CONF1_SYS_CFG2_VALUE ( \
    BCC_FLT_RST_CFG_OSC | \
    BCC_TIMEOUT_COMM_128MS | \
    BCC_EVEN_CELLS | \
    BCC_HAMM_ENCOD_DECODE \
)

/* Initial value of ADC_CFG register. */
#define BCC_CONF1_ADC_CFG_VALUE ( \
    /* Note: TAG_ID is zero. */ \
    /* Note: SOC is disable (i.e. do not initiate on-demand conversion now). */ \
    BCC_ADC2_PGA_AUTO | \
    /* Note: CC_RST is not set (do not reset CC now). */ \
    BCC_CHAR_COMP_ENABLED | \
    BCC_ADC1_A_RES_16BIT | \
    BCC_ADC1_B_RES_16BIT | \
    BCC_ADC2_RES_16BIT \
)

/* Initial value of ADC2_OFFSET_COMP register. */
#define BCC_CONF1_ADC2_OFFSET_COMP_VALUE (\
    BCC_READ_CC_RESET | \
    BCC_FREE_CC_CLAMP | \
    BCC_GET_ADC2_OFFSET(0) /* ADC2 offset compensation value. */ \
)

#ifdef MC33771
/* Initial value of OV_UV_EN register. */
#define BCC_CONF1_OV_UV_EN_VALUE ( \
    BCC_CTX_OV_TH_COMMON | \
    BCC_CTX_UV_TH_COMMON | \
    /* CTs OV and UV enable (bit is 1) or disable (bit is 0). */ \
    0x3FFFU \
)
#else
/* Initial value of OV_UV_EN register. */
#define BCC_CONF1_OV_UV_EN_VALUE ( \
    BCC_CTX_OV_TH_COMMON | \
    BCC_CTX_UV_TH_COMMON | \
    /* CTs OV and UV enable (bit is 1) or disable (bit is 0). */ \
    0x003FU \
)
#endif

/* Initial value of CELL_OV_FLT register. */
#define BCC_CONF1_CELL_OV_FLT_VALUE    0x0000U

/* Initial value of CELL_UV_FLT register. */
#define BCC_CONF1_CELL_UV_FLT_VALUE    0x0000U

/* Initial value of CBx_CFG registers. */
#define BCC_CONF1_CB1_CFG_VALUE ( \
    BCC_CB_ENABLED | \
    0xFFU /* Cell balance timer in minutes. */ \
)

#define BCC_CONF1_CB2_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB3_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB4_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB5_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB6_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#ifdef MC33771
#define BCC_CONF1_CB7_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB8_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB9_CFG_VALUE        BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB10_CFG_VALUE       BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB11_CFG_VALUE       BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB12_CFG_VALUE       BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB13_CFG_VALUE       BCC_CONF1_CB1_CFG_VALUE
#define BCC_CONF1_CB14_CFG_VALUE       BCC_CONF1_CB1_CFG_VALUE
#endif

/* Initial value of CB_SHORT_FLT register. */
#define BCC_CONF1_CB_SHORT_FLT_VALUE   0x0000U

/* Initial value of GPIO_CFG1 register. */
#define BCC_CONF1_GPIO_CFG1_VALUE ( \
    BCC_GPIOX_AN_IN_RM_MEAS(6U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(5U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(4U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(3U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(2U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(1U) | \
    BCC_GPIOX_AN_IN_RM_MEAS(0U) \
)

/* Initial value of GPIO_CFG2 register. */
#define BCC_CONF1_GPIO_CFG2_VALUE ( \
    BCC_GPIO2_ADC_TRG_DISABLED | \
    BCC_GPIO0_NO_WAKE_UP | \
    BCC_GPIO0_INP_HIGH_FP_NACT \
    /* Note: GPIOx_DR are initialized to zero (low output level). */ \
)

/* Initial value of GPIO_STS register. */
#define BCC_CONF1_GPIO_STS_VALUE       0x0000U

/* Initial value of AN_OT_UT_FLT register. */
#define BCC_CONF1_AN_OT_UT_FLT_VALUE   0x0000U

/* Initial value of GPIO_SHORT_ANx_OPEN_STS register. */
#define BCC_CONF1_GPIO_SHORT_VALUE     0x0000U

/* Initial value of FAULT3_STATUS register. */
#define BCC_CONF1_FAULT1_STATUS_VALUE  0x0000U

/* Initial value of FAULT3_STATUS register. */
#define BCC_CONF1_FAULT2_STATUS_VALUE  0x0000U

/* Initial value of FAULT3_STATUS register. */
#define BCC_CONF1_FAULT3_STATUS_VALUE  0x0000U

/* Initial value of FAULT_MASK1 register. */
#define BCC_CONF1_FAULT_MASK1_VALUE ( \
    BCC_VPWR_OV_FLT_EN | \
    BCC_VPWR_LV_FLT_EN | \
    BCC_COM_LOSS_FLT_EN | \
    BCC_COM_ERR_FLT_EN | \
    BCC_CSB_WUP_FLT_EN | \
    BCC_GPIO0_WUP_FLT_EN | \
    BCC_I2C_ERR_FLT_EN | \
    BCC_IS_OL_FLT_EN | \
    BCC_IS_OC_FLT_EN | \
    BCC_AN_OT_FLT_EN | \
    BCC_AN_UT_FLT_EN | \
    BCC_CT_OV_FLT_EN | \
    BCC_CT_UV_FLT_EN \
)

/* Initial value of FAULT_MASK2 register. */
#define BCC_CONF1_FAULT_MASK2_VALUE ( \
    BCC_VCOM_OV_FLT_EN | \
    BCC_VCOM_UV_FLT_EN | \
    BCC_VANA_OV_FLT_EN | \
    BCC_VANA_UV_FLT_EN | \
    BCC_ADC1_B_FLT_EN | \
    BCC_ADC1_A_FLT_EN | \
    BCC_GND_LOSS_FLT_EN | \
    BCC_AN_OPEN_FLT_EN | \
    BCC_GPIO_SHORT_FLT_EN | \
    BCC_CB_SHORT_FLT_EN | \
    BCC_CB_OPEN_FLT_EN | \
    BCC_OSC_ERR_FLT_EN | \
    BCC_DED_ERR_FLT_EN | \
    BCC_FUSE_ERR_FLT_EN \
)

#ifdef MC33771
/* Initial value of FAULT_MASK3 register. */
#define BCC_CONF1_FAULT_MASK3_VALUE ( \
    BCC_CC_OVR_FLT_EN | \
    BCC_DIAG_TO_FLT_EN | \
    /* CBx timeout detection (EOT_CBx bits). */ \
    BCC_EOT_CBX_FLT_DIS(1U) |                  /* CB1. */  \
    BCC_EOT_CBX_FLT_DIS(2U) |                  /* CB2. */  \
    BCC_EOT_CBX_FLT_DIS(3U) |                  /* CB3. */  \
    BCC_EOT_CBX_FLT_DIS(4U) |                  /* CB4. */  \
    BCC_EOT_CBX_FLT_DIS(5U) |                  /* CB5. */  \
    BCC_EOT_CBX_FLT_DIS(6U) |                  /* CB6. */  \
    BCC_EOT_CBX_FLT_DIS(7U) |                  /* CB7. */  \
    BCC_EOT_CBX_FLT_DIS(8U) |                  /* CB8. */  \
    BCC_EOT_CBX_FLT_DIS(9U) |                  /* CB9. */  \
    BCC_EOT_CBX_FLT_DIS(10U) |                  /* CB10. */  \
    BCC_EOT_CBX_FLT_DIS(11U) |                  /* CB11. */  \
    BCC_EOT_CBX_FLT_DIS(12U) |                  /* CB12. */  \
    BCC_EOT_CBX_FLT_DIS(13U) |                  /* CB13. */  \
    BCC_EOT_CBX_FLT_DIS(14U)                    /* CB14. */ \
)
#else
/* Initial value of FAULT_MASK3 register. */
#define BCC_CONF1_FAULT_MASK3_VALUE ( \
    BCC_CC_OVR_FLT_EN | \
    BCC_DIAG_TO_FLT_EN | \
    /* CBx timeout detection (EOT_CBx bits). */ \
    BCC_EOT_CBX_FLT_DIS(1U) |                  /* CB1. */  \
    BCC_EOT_CBX_FLT_DIS(2U) |                  /* CB2. */  \
    BCC_EOT_CBX_FLT_DIS(3U) |                  /* CB3. */  \
    BCC_EOT_CBX_FLT_DIS(4U) |                  /* CB4. */  \
    BCC_EOT_CBX_FLT_DIS(5U) |                  /* CB5. */  \
    BCC_EOT_CBX_FLT_DIS(6U)                    /* CB6. */  \
)
#endif

/* Initial value of WAKEUP_MASK1 register. */
#define BCC_CONF1_WAKEUP_MASK1_VALUE ( \
    BCC_VPWR_OV_WAKEUP_EN | \
    BCC_VPWR_LV_WAKEUP_EN | \
    BCC_CSB_WUP_WAKEUP_EN | \
    BCC_GPIO0_WUP_WAKEUP_EN | \
    BCC_IS_OC_WAKEUP_EN | \
    BCC_AN_OT_WAKEUP_EN | \
    BCC_AN_UT_WAKEUP_EN | \
    BCC_CT_OV_WAKEUP_EN | \
    BCC_CT_UV_WAKEUP_EN \
)

/* Initial value of WAKEUP_MASK2 register. */
#define BCC_CONF1_WAKEUP_MASK2_VALUE ( \
    BCC_VCOM_OV_WAKEUP_EN | \
    BCC_VCOM_UV_WAKEUP_EN | \
    BCC_VANA_OV_WAKEUP_EN | \
    BCC_VANA_UV_WAKEUP_EN | \
    BCC_ADC1_B_WAKEUP_EN | \
    BCC_ADC1_A_WAKEUP_EN | \
    BCC_GND_LOSS_WAKEUP_EN | \
    BCC_IC_TSD_WAKEUP_EN | \
    BCC_GPIO_SHORT_WAKEUP_EN | \
    BCC_CB_SHORT_WAKEUP_EN | \
    BCC_OSC_ERR_WAKEUP_EN | \
    BCC_DED_ERR_WAKEUP_EN \
)

#ifdef MC33771
/* Initial value of WAKEUP_MASK3 register. */
#define BCC_CONF1_WAKEUP_MASK3_VALUE ( \
    BCC_CC_OVR_FLT_EN | \
    BCC_DIAG_TO_FLT_DIS | \
    /* CBx timeout detection (EOT_CBx bits). */ \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB1. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB2. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB3. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB4. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB5. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB6. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB7. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB8. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB9. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB10. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB11. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB12. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB13. */  \
    BCC_EOT_CBX_WAKEUP_EN                    /* CB14. */ \
)
#else
/* Initial value of WAKEUP_MASK3 register. */
#define BCC_CONF1_WAKEUP_MASK3_VALUE ( \
    BCC_CC_OVR_FLT_EN | \
    BCC_DIAG_TO_FLT_DIS | \
    /* CBx timeout detection (EOT_CBx bits). */ \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB1. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB2. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB3. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB4. */  \
    BCC_EOT_CBX_WAKEUP_EN |                  /* CB5. */  \
    BCC_EOT_CBX_WAKEUP_EN                    /* CB6. */  \
)
#endif

/* Initial value of TH_ALL_CT register. */
#define BCC_CONF1_TH_ALL_CT_VALUE ( \
    BCC_SET_ALL_CT_OV_TH(1510U)  /* CT OV threshold is 1510 mV. It is enabled/disabled through OV_UV_EN register. */ | \
    BCC_SET_ALL_CT_UV_TH(1000U)  /* CT UV threshold is 1000 mV. It is enabled/disabled through OV_UV_EN register. */ \
)

/* Initial value of TH_CTx registers. */
#define BCC_CONF1_TH_CT1_VALUE ( \
    BCC_SET_CTX_OV_TH(4195U)  /* CT OV threshold is 4195 mV. It is enabled/disabled through OV_UV_EN register. */ | \
    BCC_SET_CTX_UV_TH(2509U)  /* CT UV threshold is 2509 mV. It is enabled/disabled through OV_UV_EN register. */ \
)

#define BCC_CONF1_TH_CT2_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT3_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT4_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT5_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT6_VALUE         BCC_CONF1_TH_CT1_VALUE
#ifdef MC33771
#define BCC_CONF1_TH_CT7_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT8_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT9_VALUE         BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT10_VALUE        BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT11_VALUE        BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT12_VALUE        BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT13_VALUE        BCC_CONF1_TH_CT1_VALUE
#define BCC_CONF1_TH_CT14_VALUE        BCC_CONF1_TH_CT1_VALUE
#endif

/* Initial value of TH_ANx_OT registers. */
#define BCC_CONF1_TH_AN0_OT_VALUE ( \
    BCC_SET_ANX_OT_TH(1160U)  /* AN OT threshold is 1160 mV. It is enabled/disabled through FAULT_MASK1 register. */ \
)

#define BCC_CONF1_TH_AN1_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE
#define BCC_CONF1_TH_AN2_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE
#define BCC_CONF1_TH_AN3_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE
#define BCC_CONF1_TH_AN4_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE
#define BCC_CONF1_TH_AN5_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE
#define BCC_CONF1_TH_AN6_OT_VALUE      BCC_CONF1_TH_AN0_OT_VALUE

/* Initial value of TH_ANx_UV registers. */
#define BCC_CONF1_TH_AN0_UT_VALUE ( \
    BCC_SET_ANX_UT_TH(3820U)  /* AN UT threshold is 3820 mV. It is enabled/disabled through FAULT_MASK1 register. */ \
)

#define BCC_CONF1_TH_AN1_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE
#define BCC_CONF1_TH_AN2_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE
#define BCC_CONF1_TH_AN3_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE
#define BCC_CONF1_TH_AN4_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE
#define BCC_CONF1_TH_AN5_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE
#define BCC_CONF1_TH_AN6_UT_VALUE      BCC_CONF1_TH_AN0_UT_VALUE

/* Initial value of TH_ISENSE_OC register. */
#define BCC_CONF1_TH_ISENSE_OC_VALUE ( \
    /* ISENSE OC threshold is 24576 mA (2458 uV using 100 uOhm resistor). It is enabled/disabled through FAULT_MASK1 and WAKEUP_MASK1 register. */ \
    BCC_SET_TH_ISENSE_OC(2458U) \
)

/* Initial value of TH_COULOMB_CNT_MSB register. */
#define BCC_CONF1_TH_COULOMB_CNT_MSB_VALUE ( \
    BCC_SET_TH_COULOMB_CNT_MSB(0U) /* Higher 16 bits of over Coulomb threshold (2's complement representation). */ \
)

/* Initial value of TH_COULOMB_CNT_LSB register. */
#define BCC_CONF1_TH_COULOMB_CNT_LSB_VALUE ( \
    BCC_SET_TH_COULOMB_CNT_LSB(0U) /* Lower 16 bits of over Coulomb threshold (2's complement representation). */ \
)

/*******************************************************************************
 * Structure definition
 ******************************************************************************/

/* Structure containing all data related to the Battery Cell Controller. */
typedef struct
{
    bcc_drv_config_t drvConfig;           /* BCC driver configuration. */
} bcc_data_t;

/*******************************************************************************
 * Global variables
 ******************************************************************************/

extern bcc_data_t g_bccData;

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @brief This function prints content of registers configured in initialization
 * phase to serial console output.
 *
 * @param cid Cluster Identification Address.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t printInitialSettings(uint8_t cid);

/*!
 * @brief This function prints content of fault registers to serial console
 * output.
 *
 * @param cid Cluster Identification Address.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t printFaultRegisters(uint8_t cid);

#endif /* COMMON_H_ */
