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

#include "utils/nxp_console.h" /* PRINTF */
#include "common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Value of CELL_OV_FLT register when no event occurred. */
#define BCC_CELL_OV_FLT_NOEVENT   0x0000U

/* Value of CELL_UV_FLT register when no event occurred. */
#define BCC_CELL_UV_FLT_NOEVENT   0x0000U

/* Value of CB_OPEN_FLT register when no event occurred. */
#define BCC_CB_OPEN_FLT_NOEVENT   0x0000U

/* Value of CB_SHORT_FLT register when no event occurred. */
#define BCC_CB_SHORT_FLT_NOEVENT  0x0000U

/* Value of AN_OT_UT_FLT register when no event occurred. */
#define BCC_AN_OT_UT_FLT_NOEVENT  0x0000U

/* Value of GPIO_SHORT register when no event occurred. */
#define BCC_GPIO_SHORT_NOEVENT    0x0000U

/* Value of FAULT1_STATUS register when no event occurred. */
#define BCC_FAULT1_STATUS_NOEVENT 0x0000U

/* Value of FAULT2_STATUS register when no event occurred. */
#define BCC_FAULT2_STATUS_NOEVENT 0x0000U

/* Value of FAULT3_STATUS register when no event occurred. */
#define BCC_FAULT3_STATUS_NOEVENT 0x0000U

/* Address of last printed fuse mirror register. */
#define BCC_LAST_FUSE_ADDR_MC33771B   0x1A
#define BCC_LAST_FUSE_ADDR_MC33772B   0x12

/**
 * Prints formated string with register name, value and whether an event
 * occurred. It is intended for the status registers.
 *
 * @param regName Name of a status register.
 * @param regVal Value of a status register.
 * @param event "yes" when an event is signalized by register value.
 */
#define PRINT_STATUS_REG(regName, regVal, event) \
    (PRINTF("  | %s\t| 0x%02x %02x\t| %s\t\t     |\r\n", \
            regName, regVal >> 8, regVal & 0x00FF, event))

/**
 * Prints formated string with register name, value and whether an event
 * occurred. It is intended for the status registers.
 *
 * @param regName Name of a status register.
 * @param regVal Value of a status register.
 * @param defVal Value of a status register when has not occurred any event.
 */
#define PRINT_STATUS_REG_COMP(regName, regVal, defVal) \
    (PRINT_STATUS_REG(regName, regVal, (regVal != defVal) ? "yes" : "no"))

/* Number of configurable registers. */
#define REG_CONF_CNT_MC33771    60
#define REG_CONF_CNT_MC33772    44

/*******************************************************************************
 * Structure definition
 ******************************************************************************/

/* Structure containing a register name and its address. */
typedef struct
{
    const char* name;
    const uint8_t address;
} bcc_drv_register_t;

/*******************************************************************************
 * Global variables (constants)
 ******************************************************************************/

/* Names and addresses of all configurable registers. */
const bcc_drv_register_t BCC_REGISTERS_DATA_MC33771[REG_CONF_CNT_MC33771] = {
    /* Note: INIT register is initialized automatically. */
    /* Note: SYS_CFG_GLOBAL reg. contains only command GO2SLEEP
     * (no initialization needed). */
    { "SYS_CFG1", BCC_REG_SYS_CFG1_ADDR },
    { "SYS_CFG2", BCC_REG_SYS_CFG2_ADDR },
    { "SYS_DIAG", BCC_REG_SYS_DIAG_ADDR },
    { "ADC_CFG", BCC_REG_ADC_CFG_ADDR },
    { "ADC2_OFFSET_COMP", BCC_REG_ADC2_OFFSET_COMP_ADDR },
    { "OV_UV_EN", BCC_REG_OV_UV_EN_ADDR },
    { "CB1_CFG", BCC_REG_CB1_CFG_ADDR },
    { "CB2_CFG", BCC_REG_CB2_CFG_ADDR },
    { "CB3_CFG", BCC_REG_CB3_CFG_ADDR },
    { "CB4_CFG", BCC_REG_CB4_CFG_ADDR },
    { "CB5_CFG", BCC_REG_CB5_CFG_ADDR },
    { "CB6_CFG", BCC_REG_CB6_CFG_ADDR },
    { "CB7_CFG", BCC_REG_CB7_CFG_ADDR },
    { "CB8_CFG", BCC_REG_CB8_CFG_ADDR },
    { "CB9_CFG", BCC_REG_CB9_CFG_ADDR },
    { "CB10_CFG", BCC_REG_CB10_CFG_ADDR },
    { "CB11_CFG", BCC_REG_CB11_CFG_ADDR },
    { "CB12_CFG", BCC_REG_CB12_CFG_ADDR },
    { "CB13_CFG", BCC_REG_CB13_CFG_ADDR },
    { "CB14_CFG", BCC_REG_CB14_CFG_ADDR },
    { "GPIO_CFG1", BCC_REG_GPIO_CFG1_ADDR },
    { "GPIO_CFG2", BCC_REG_GPIO_CFG2_ADDR },
    { "FAULT_MASK1", BCC_REG_FAULT_MASK1_ADDR },
    { "FAULT_MASK2", BCC_REG_FAULT_MASK2_ADDR },
    { "FAULT_MASK3", BCC_REG_FAULT_MASK3_ADDR },
    { "WAKEUP_MASK1", BCC_REG_WAKEUP_MASK1_ADDR },
    { "WAKEUP_MASK2", BCC_REG_WAKEUP_MASK2_ADDR },
    { "WAKEUP_MASK3", BCC_REG_WAKEUP_MASK3_ADDR },
    { "TH_ALL_CT", BCC_REG_TH_ALL_CT_ADDR },
    { "TH_CT14", BCC_REG_TH_CT14_ADDR },
    { "TH_CT13", BCC_REG_TH_CT13_ADDR },
    { "TH_CT12", BCC_REG_TH_CT12_ADDR },
    { "TH_CT11", BCC_REG_TH_CT11_ADDR },
    { "TH_CT10", BCC_REG_TH_CT10_ADDR },
    { "TH_CT9", BCC_REG_TH_CT9_ADDR },
    { "TH_CT8", BCC_REG_TH_CT8_ADDR },
    { "TH_CT7", BCC_REG_TH_CT7_ADDR },
    { "TH_CT6", BCC_REG_TH_CT6_ADDR },
    { "TH_CT5", BCC_REG_TH_CT5_ADDR },
    { "TH_CT4", BCC_REG_TH_CT4_ADDR },
    { "TH_CT3", BCC_REG_TH_CT3_ADDR },
    { "TH_CT2", BCC_REG_TH_CT2_ADDR },
    { "TH_CT1", BCC_REG_TH_CT1_ADDR },
    { "TH_AN6_OT", BCC_REG_TH_AN6_OT_ADDR },
    { "TH_AN5_OT", BCC_REG_TH_AN5_OT_ADDR },
    { "TH_AN4_OT", BCC_REG_TH_AN4_OT_ADDR },
    { "TH_AN3_OT", BCC_REG_TH_AN3_OT_ADDR },
    { "TH_AN2_OT", BCC_REG_TH_AN2_OT_ADDR },
    { "TH_AN1_OT", BCC_REG_TH_AN1_OT_ADDR },
    { "TH_AN0_OT", BCC_REG_TH_AN0_OT_ADDR },
    { "TH_AN6_UT", BCC_REG_TH_AN6_UT_ADDR },
    { "TH_AN5_UT", BCC_REG_TH_AN5_UT_ADDR },
    { "TH_AN4_UT", BCC_REG_TH_AN4_UT_ADDR },
    { "TH_AN3_UT", BCC_REG_TH_AN3_UT_ADDR },
    { "TH_AN2_UT", BCC_REG_TH_AN2_UT_ADDR },
    { "TH_AN1_UT", BCC_REG_TH_AN1_UT_ADDR },
    { "TH_AN0_UT", BCC_REG_TH_AN0_UT_ADDR },
    { "TH_ISENSE OC", BCC_REG_TH_ISENSE_OC_ADDR },
    { "TH_COULOMB_CNT_MSB", BCC_REG_TH_COULOMB_CNT_MSB_ADDR },
    { "TH_COULOMB_CNT_LSB", BCC_REG_TH_COULOMB_CNT_LSB_ADDR }
};

const bcc_drv_register_t BCC_REGISTERS_DATA_MC33772[REG_CONF_CNT_MC33772] = {
    /* Note: INIT register is initialized automatically. */
    /* Note: SYS_CFG_GLOBAL reg. contains only command GO2SLEEP
     * (no initialization needed). */
    { "SYS_CFG1", BCC_REG_SYS_CFG1_ADDR },
    { "SYS_CFG2", BCC_REG_SYS_CFG2_ADDR },
    { "SYS_DIAG", BCC_REG_SYS_DIAG_ADDR },
    { "ADC_CFG", BCC_REG_ADC_CFG_ADDR },
    { "ADC2_OFFSET_COMP", BCC_REG_ADC2_OFFSET_COMP_ADDR },
    { "OV_UV_EN", BCC_REG_OV_UV_EN_ADDR },
    { "CB1_CFG", BCC_REG_CB1_CFG_ADDR },
    { "CB2_CFG", BCC_REG_CB2_CFG_ADDR },
    { "CB3_CFG", BCC_REG_CB3_CFG_ADDR },
    { "CB4_CFG", BCC_REG_CB4_CFG_ADDR },
    { "CB5_CFG", BCC_REG_CB5_CFG_ADDR },
    { "CB6_CFG", BCC_REG_CB6_CFG_ADDR },
    { "GPIO_CFG1", BCC_REG_GPIO_CFG1_ADDR },
    { "GPIO_CFG2", BCC_REG_GPIO_CFG2_ADDR },
    { "FAULT_MASK1", BCC_REG_FAULT_MASK1_ADDR },
    { "FAULT_MASK2", BCC_REG_FAULT_MASK2_ADDR },
    { "FAULT_MASK3", BCC_REG_FAULT_MASK3_ADDR },
    { "WAKEUP_MASK1", BCC_REG_WAKEUP_MASK1_ADDR },
    { "WAKEUP_MASK2", BCC_REG_WAKEUP_MASK2_ADDR },
    { "WAKEUP_MASK3", BCC_REG_WAKEUP_MASK3_ADDR },
    { "TH_ALL_CT", BCC_REG_TH_ALL_CT_ADDR },
    { "TH_CT6", BCC_REG_TH_CT6_ADDR },
    { "TH_CT5", BCC_REG_TH_CT5_ADDR },
    { "TH_CT4", BCC_REG_TH_CT4_ADDR },
    { "TH_CT3", BCC_REG_TH_CT3_ADDR },
    { "TH_CT2", BCC_REG_TH_CT2_ADDR },
    { "TH_CT1", BCC_REG_TH_CT1_ADDR },
    { "TH_AN6_OT", BCC_REG_TH_AN6_OT_ADDR },
    { "TH_AN5_OT", BCC_REG_TH_AN5_OT_ADDR },
    { "TH_AN4_OT", BCC_REG_TH_AN4_OT_ADDR },
    { "TH_AN3_OT", BCC_REG_TH_AN3_OT_ADDR },
    { "TH_AN2_OT", BCC_REG_TH_AN2_OT_ADDR },
    { "TH_AN1_OT", BCC_REG_TH_AN1_OT_ADDR },
    { "TH_AN0_OT", BCC_REG_TH_AN0_OT_ADDR },
    { "TH_AN6_UT", BCC_REG_TH_AN6_UT_ADDR },
    { "TH_AN5_UT", BCC_REG_TH_AN5_UT_ADDR },
    { "TH_AN4_UT", BCC_REG_TH_AN4_UT_ADDR },
    { "TH_AN3_UT", BCC_REG_TH_AN3_UT_ADDR },
    { "TH_AN2_UT", BCC_REG_TH_AN2_UT_ADDR },
    { "TH_AN1_UT", BCC_REG_TH_AN1_UT_ADDR },
    { "TH_AN0_UT", BCC_REG_TH_AN0_UT_ADDR },
    { "TH_ISENSE OC", BCC_REG_TH_ISENSE_OC_ADDR },
    { "TH_COULOMB_CNT_MSB", BCC_REG_TH_COULOMB_CNT_MSB_ADDR },
    { "TH_COULOMB_CNT_LSB", BCC_REG_TH_COULOMB_CNT_LSB_ADDR }
};

/*******************************************************************************
 * API
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : printInitialSettings
 * Description   : This function prints content of registers configured in
 *                 initialization phase to serial console output.
 *
 *END**************************************************************************/
bcc_status_t printInitialSettings(uint8_t cid)
{
    uint64_t guid;
    uint16_t regVal;
    uint8_t i;
    static char* printPattern = "  | %-18s | 0x%02X%02X |\r\n";
    bcc_status_t error;

    PRINTF("###############################################\r\n");
    PRINTF("# CID %d (MC3377%s): Initial value of registers\r\n", cid,
            (g_bccData.drvConfig.device[cid - 1] == BCC_DEVICE_MC33771) ?
                    "1" : "2");
    PRINTF("###############################################\r\n\r\n");

    PRINTF("  -------------------------------\r\n");
    PRINTF("  | Register           | Value  |\r\n");
    PRINTF("  -------------------------------\r\n");

    error = BCC_Reg_Read(&g_bccData.drvConfig, cid, BCC_REG_INIT_ADDR, 1U, &regVal);
    if (error != BCC_STATUS_SUCCESS)
    {
        return error;
    }

    PRINTF(printPattern, "INIT", regVal >> 8, regVal & 0xFFU);

    if (g_bccData.drvConfig.device[cid - 1] == BCC_DEVICE_MC33771)
    {
        for (i = 0U; i < REG_CONF_CNT_MC33771; i++)
        {
            error = BCC_Reg_Read(&g_bccData.drvConfig, cid,
                                 BCC_REGISTERS_DATA_MC33771[i].address, 1U, &regVal);
            if (error != BCC_STATUS_SUCCESS)
            {
                return error;
            }
            PRINTF(printPattern, BCC_REGISTERS_DATA_MC33771[i].name,
                    regVal >> 8, regVal & 0xFFU);
        }
    }
    else
    {
        for (i = 0U; i < REG_CONF_CNT_MC33772; i++)
        {
            error = BCC_Reg_Read(&g_bccData.drvConfig, cid,
                                 BCC_REGISTERS_DATA_MC33772[i].address, 1U, &regVal);
            if (error != BCC_STATUS_SUCCESS)
            {
                return error;
            }
            PRINTF(printPattern, BCC_REGISTERS_DATA_MC33772[i].name, regVal >> 8, regVal & 0xFFU);
        }
    }

    PRINTF("  ----------------------------------\r\n");
    PRINTF("\r\n");

    PRINTF("  ------------------------\r\n");
    PRINTF("  | Fuse Mirror | Value  |\r\n");
    PRINTF("  |  Register   |        |\r\n");
    PRINTF("  ------------------------\r\n");
    for (i = 0U; i <= ((g_bccData.drvConfig.device[cid - 1] == BCC_DEVICE_MC33771) ?
            BCC_LAST_FUSE_ADDR_MC33771B : BCC_LAST_FUSE_ADDR_MC33772B); i++)
    {
        error = BCC_FuseMirror_Read(&g_bccData.drvConfig, cid, i, &regVal);
        if (error != BCC_STATUS_SUCCESS)
        {
            return error;
        }

        PRINTF("  | $%02X\t| 0x%02X%02X |\r\n", i, regVal >> 8, regVal & 0xFFU);
    }
    PRINTF("  ------------------------\r\n");
    PRINTF("\r\n");

    error = BCC_GUID_Read(&g_bccData.drvConfig, cid, &guid);
    if (error != BCC_STATUS_SUCCESS)
    {
        return error;
    }

    PRINTF("  Device GUID: %02X%04X%04X\r\n",
            (uint16_t)((guid >> 32) & 0x001FU),
            (uint16_t)((guid >> 16) & 0xFFFFU),
            (uint16_t)(guid & 0xFFFFU));
    PRINTF("\r\n");

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : printFaultRegisters
 * Description   : This function prints content of fault registers to serial
 *                 console output.
 *
 *END**************************************************************************/
bcc_status_t printFaultRegisters(uint8_t cid)
{
    uint16_t status[BCC_STAT_CNT]; /* Status registers. */
    bcc_status_t error;

    PRINTF("###############################################\r\n");
    PRINTF("# CID %d (MC3377%s): Device status\r\n", cid,
            (g_bccData.drvConfig.device[cid - 1] == BCC_DEVICE_MC33771) ?
                    "1" : "2");
    PRINTF("###############################################\r\n\r\n");

    error = BCC_Fault_GetStatus(&g_bccData.drvConfig, cid, status);
    if (error != BCC_STATUS_SUCCESS)
    {
        return error;
    }

    PRINTF("  ----------------------------------------------------\r\n");
    PRINTF("  | Status\t| Raw Value\t| An event occurred? |\r\n");
    PRINTF("  ----------------------------------------------------\r\n");

    PRINT_STATUS_REG_COMP("CELL_OV", status[BCC_FS_CELL_OV],
            BCC_CELL_OV_FLT_NOEVENT);
    PRINT_STATUS_REG_COMP("CELL_UV", status[BCC_FS_CELL_UV],
            BCC_CELL_UV_FLT_NOEVENT);
    PRINT_STATUS_REG_COMP("CB_OPEN", status[BCC_FS_CB_OPEN],
            BCC_CB_OPEN_FLT_NOEVENT);
    PRINT_STATUS_REG_COMP("CB_SHORT", status[BCC_FS_CB_SHORT],
            BCC_CB_SHORT_FLT_NOEVENT);
    PRINT_STATUS_REG_COMP("AN_OT_UT", status[BCC_FS_AN_OT_UT],
            BCC_AN_OT_UT_FLT_NOEVENT);
    PRINT_STATUS_REG_COMP("GPIO_SHORT", status[BCC_FS_GPIO_SHORT],
            BCC_GPIO_SHORT_NOEVENT);
    PRINT_STATUS_REG_COMP("FAULT1", status[BCC_FS_FAULT1],
            BCC_FAULT1_STATUS_NOEVENT);
    PRINT_STATUS_REG_COMP("FAULT2", status[BCC_FS_FAULT2],
            BCC_FAULT2_STATUS_NOEVENT);
    PRINT_STATUS_REG_COMP("FAULT3", status[BCC_FS_FAULT3],
            BCC_FAULT3_STATUS_NOEVENT);

    /* Note: GPIO_STS is not a fault register. */
    PRINT_STATUS_REG("GPIO_STS", status[BCC_FS_GPIO_STATUS], "-");

    /* Note: COM_STATUS is not a fault register. */
    PRINT_STATUS_REG("COM_STATUS", status[BCC_FS_COMM], "-");

    PRINTF("  ----------------------------------------------------\r\n\r\n");

    /* Notes:
    * - The fault bits in the CELL_OV register are ORed to the FAULT1_STATUS
    * [CT_OV_FLT] bit.
    * - The fault bits in the CELL_UV register are ORed to the FAULT1_STATUS
    * [CT_UV_FLT] bit.
    */

    return BCC_STATUS_SUCCESS;
}
