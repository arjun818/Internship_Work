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

/*
 * File: bcc_peripheries.c
 *
 * This file implements functions for LPSPI and GPIO operations required by BCC
 * driver. It is closely related to this demo example.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "Cpu.h"
#include "bcc_peripheries.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Number of bytes what 40b needs to be aligned in S32K144 SDK LPSPI driver
 * to. */
#define LPSPI_ALIGNMENT   8

/*******************************************************************************
 * Global variables (constants)
 ******************************************************************************/

/**
 * LPSPI State structure for each LPSPI instance.
 */
lpspi_state_t g_lpspiState[LPSPI_INSTANCE_COUNT];

/*******************************************************************************
 * Prototypes of internal functions
 ******************************************************************************/

/*!
 * @brief This function sends 40b data via TX SPI bus. It handles CSB_TX
 * by itself. Intended for TPL mode only.
 *
 * @param drvInstance BCC driver instance.
 * @param buff Data to be sent.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t BCC_TransferTxTpl(uint8_t drvInstance, uint8_t transBuf[]);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_TransferTxTpl
 * Description   : This function sends data via TX SPI bus. It handles CSB_TX
 *                 by itself. Intended for TPL mode only.
 *
 *END**************************************************************************/
static bcc_status_t BCC_TransferTxTpl(uint8_t drvInstance, uint8_t transBuf[])
{
    uint8_t tBuf[LPSPI_ALIGNMENT];
    status_t error;

    /* Buffer transBuf needs to be aligned to a multiple of 4B in S32K1xx LPSPI
     * SDK 3.0.0 RTM. Buffer tBuf is used for this purpose. */
    tBuf[0] = transBuf[3];
    tBuf[1] = transBuf[2];
    tBuf[2] = transBuf[1];
    tBuf[3] = transBuf[0];
    tBuf[4] = transBuf[4];

#if defined(TPL_TRANSLT)
    /* Select CSB. */
    PINS_DRV_ClearPins(BCC_TPL_TX_PCS_INSTANCE, 1U << BCC_TPL_TX_PCS_PIN);

    /* Send data. */
    error = LPSPI_DRV_MasterTransferBlocking(BCC_TPL_TX_LPSPI_INSTANCE, tBuf, NULL,
            LPSPI_ALIGNMENT, BCC_COM_TIMEOUT_MS);
    if (error != STATUS_SUCCESS)
    {
        /* Unselect slave. */
        PINS_DRV_SetPins(BCC_TPL_TX_PCS_INSTANCE, 1U << BCC_TPL_TX_PCS_PIN);

        return (error == STATUS_TIMEOUT) ? BCC_STATUS_COM_TIMEOUT : BCC_STATUS_SPI_BUSY;
    }

    /* SPI TX_CS must go high after receiving an echo frame from MC33664.
     * Wait for the end of received frame. */
    BCC_MCU_WaitUs(2U);

    /* Unselect CSB. */
    PINS_DRV_SetPins(BCC_TPL_TX_PCS_INSTANCE, 1U << BCC_TPL_TX_PCS_PIN);
#else
    /* Send data. */
    error = LPSPI_DRV_MasterTransferBlocking(BCC_TPL_TX_LPSPI_INSTANCE, tBuf, NULL,
            LPSPI_ALIGNMENT, BCC_COM_TIMEOUT_MS);
    if (error != STATUS_SUCCESS)
    {
        return (error == STATUS_TIMEOUT) ? BCC_STATUS_COM_TIMEOUT : BCC_STATUS_SPI_BUSY;
    }
#endif

    return BCC_STATUS_SUCCESS;
}

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_ConfigureLPSPI
 * Description   : This function configures LPSPI for usage with BCC driver.
 *
 *END**************************************************************************/
status_t BCC_MCU_ConfigureLPSPI()
{
    lpspi_master_config_t bccSpiSdkMasterConfig;
#if defined(TPL) || defined(TPL_TRANSLT)
    lpspi_slave_config_t bccSpiSdkSlaveConfig;
#endif
    status_t status;

    /* Master SPI interface. */
#if defined(TPL) || defined(TPL_TRANSLT)
    if ((BCC_TPL_TX_LPSPI_BAUD < BCC_SPI_FREQ_MC33664_MIN) ||
        (BCC_TPL_TX_LPSPI_BAUD > BCC_SPI_FREQ_MC33664_MAX))
    {
        return STATUS_UNSUPPORTED;
    }
#else
    if (BCC_SPI_LPSPI_BAUD > BCC_SPI_FREQ_MC3377x_MAX)
    {
        return STATUS_UNSUPPORTED;
    }
#endif

    bccSpiSdkMasterConfig.bitcount = 40U;
    bccSpiSdkMasterConfig.callback = NULL;
    bccSpiSdkMasterConfig.callbackParam = NULL;
    bccSpiSdkMasterConfig.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE;
    bccSpiSdkMasterConfig.clkPolarity = LPSPI_ACTIVE_LOW;
    bccSpiSdkMasterConfig.isPcsContinuous = false;
    bccSpiSdkMasterConfig.lsbFirst= false;
    bccSpiSdkMasterConfig.pcsPolarity = LPSPI_ACTIVE_LOW;
    bccSpiSdkMasterConfig.rxDMAChannel = 2U;
    bccSpiSdkMasterConfig.transferType = LPSPI_USING_INTERRUPTS;
    bccSpiSdkMasterConfig.txDMAChannel = 2U;

#if defined(SPI)
    /* Master SPI initialization for SPI BCC EVBs. */
    bccSpiSdkMasterConfig.bitsPerSec = BCC_SPI_LPSPI_BAUD;
    bccSpiSdkMasterConfig.lpspiSrcClk = BCC_SPI_LPSPI_SRCCLK;
    bccSpiSdkMasterConfig.whichPcs = BCC_SPI_LPSPI_PCS;

    status = LPSPI_DRV_MasterInit(BCC_SPI_LPSPI_INSTANCE,
                &(g_lpspiState[BCC_SPI_LPSPI_INSTANCE]), &bccSpiSdkMasterConfig);
    if (status != STATUS_SUCCESS)
    {
        return status;
    }
#elif defined(TPL) || defined(TPL_TRANSLT)
    /* Master SPI initialization for TPL BCC EVBs. */
    bccSpiSdkMasterConfig.bitsPerSec = BCC_TPL_TX_LPSPI_BAUD;
    bccSpiSdkMasterConfig.lpspiSrcClk = BCC_TPL_TX_LPSPI_SRCCLK;

#if defined(TPL)
    bccSpiSdkMasterConfig.whichPcs = BCC_TPL_TX_LPSPI_PCS;
#else
    bccSpiSdkMasterConfig.whichPcs = LPSPI_PCS0;
#endif

    status = LPSPI_DRV_MasterInit(BCC_TPL_TX_LPSPI_INSTANCE,
                &(g_lpspiState[BCC_TPL_TX_LPSPI_INSTANCE]), &bccSpiSdkMasterConfig);
    if (status != STATUS_SUCCESS)
    {
        return status;
    }

#if defined(TPL)
    status = LPSPI_DRV_MasterSetDelay(BCC_TPL_TX_LPSPI_INSTANCE, BCC_TPL_TX_LPSPI_DELAY_BETWEEN_TRANSFERS,
            BCC_TPL_TX_LPSPI_DELAY_SCLK_TO_PCS, BCC_TPL_TX_LPSPI_DELAY_PCS_TO_SCLK);
    if (status != STATUS_SUCCESS)
    {
        return status;
    }
#else
    /* Set CSB (TX SPI in TPL mode) pin direction and initial value. */
    PINS_DRV_SetPinDirection(BCC_TPL_TX_PCS_INSTANCE, BCC_TPL_TX_PCS_PIN, 1);
    PINS_DRV_SetPins(BCC_TPL_TX_PCS_INSTANCE, 1U << BCC_TPL_TX_PCS_PIN);
#endif

#endif

    /* Slave SPI interface - only in TPL mode. */
#if defined(TPL) || defined(TPL_TRANSLT)
    bccSpiSdkSlaveConfig.bitcount = 40U;
    bccSpiSdkSlaveConfig.callback = NULL;
    bccSpiSdkSlaveConfig.callbackParam = NULL;
    bccSpiSdkSlaveConfig.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE;
    bccSpiSdkSlaveConfig.clkPolarity = LPSPI_ACTIVE_LOW;
    bccSpiSdkSlaveConfig.lsbFirst = false;
    bccSpiSdkSlaveConfig.pcsPolarity = LPSPI_ACTIVE_LOW;
    bccSpiSdkSlaveConfig.rxDMAChannel = 2U;
    bccSpiSdkSlaveConfig.transferType = LPSPI_USING_INTERRUPTS;
    bccSpiSdkSlaveConfig.txDMAChannel = 2U;
    bccSpiSdkSlaveConfig.whichPcs = BCC_TPL_RX_LPSPI_PCS;

    status = LPSPI_DRV_SlaveInit(BCC_TPL_RX_LPSPI_INSTANCE,
                &(g_lpspiState[BCC_TPL_RX_LPSPI_INSTANCE]), &bccSpiSdkSlaveConfig);
    if (status != STATUS_SUCCESS)
    {
        return status;
    }
#endif

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_TransferSpi
 * Description   : This function sends and receives data via SPI bus. Intended
 *                 for SPI mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_MCU_TransferSpi(uint8_t drvInstance, uint8_t transBuf[],
        uint8_t recvBuf[])
{
    uint8_t tBuf[LPSPI_ALIGNMENT];
    uint8_t rBuf[LPSPI_ALIGNMENT];
    status_t error;

    DEV_ASSERT(transBuf != NULL);
    DEV_ASSERT(recvBuf != NULL);

    /* Buffers transBuf and recvBuf need to be aligned to a multiple of 4B
     * in S32K1xx LPSPI SDK 3.0.0 RTM. Buffers tBuf and rBuf are used for this
     * purpose. */
    tBuf[0] = transBuf[3];
    tBuf[1] = transBuf[2];
    tBuf[2] = transBuf[1];
    tBuf[3] = transBuf[0];
    tBuf[4] = transBuf[4];

    error = LPSPI_DRV_MasterTransferBlocking(BCC_SPI_LPSPI_INSTANCE, tBuf, rBuf,
            LPSPI_ALIGNMENT, BCC_COM_TIMEOUT_MS);
    if (error != STATUS_SUCCESS)
    {
        return (error == STATUS_TIMEOUT) ? BCC_STATUS_COM_TIMEOUT : BCC_STATUS_SPI_BUSY;
    }

    /* Store received data to recvBuf. */
    recvBuf[0] = rBuf[3];
    recvBuf[1] = rBuf[2];
    recvBuf[2] = rBuf[1];
    recvBuf[3] = rBuf[0];
    recvBuf[4] = rBuf[4];

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_TransferTpl
 * Description   : This function sends and receives data via TX and RX SPI buses.
 *                 Intended for TPL mode only.
 *
 *END**************************************************************************/
bcc_status_t BCC_MCU_TransferTpl(uint8_t drvInstance, uint8_t transBuf[],
        uint8_t recvBuf[], uint16_t recvTrCnt)
{
    uint8_t rBuf[BCC_RX_BUF_SIZE_TPL / BCC_MSG_SIZE * LPSPI_ALIGNMENT];
    bcc_status_t bccError;
    status_t error;
    uint8_t i;
    /* Communication timeout. */
    uint32_t timeout = (BCC_MCU_GetSystemClockFreq() / 1000U * BCC_COM_TIMEOUT_MS);

    DEV_ASSERT(transBuf != NULL);
    DEV_ASSERT(recvBuf != NULL);

    /* Buffers transBuf and recvBuf need to be aligned to a multiple of 4B
     * in S32K1xx LPSPI SDK 3.0.0 RTM. Buffer rBuf is used for this purpose. */

    /* Transmissions at RX and TX SPI occur almost at the same time. Start
     * reading (response) at RX SPI first. */
    error = LPSPI_DRV_SlaveTransfer(BCC_TPL_RX_LPSPI_INSTANCE, NULL, rBuf,
            recvTrCnt * LPSPI_ALIGNMENT);
    if (error != STATUS_SUCCESS)
    {
        return BCC_STATUS_SPI_BUSY;
    }

    /* Send data via TX SPI. */
    if ((bccError = BCC_TransferTxTpl(drvInstance, transBuf)) != BCC_STATUS_SUCCESS)
    {
        /* Cancel reception of data. */
        LPSPI_DRV_SlaveAbortTransfer(BCC_TPL_RX_LPSPI_INSTANCE);
        return bccError;
    }

    /* Wait until RX transmission finished. */
    while ((LPSPI_DRV_SlaveGetTransferStatus(BCC_TPL_RX_LPSPI_INSTANCE, NULL)
            == STATUS_BUSY) && (timeout > 0U))
    {
        timeout--;
    }

    /* Cancel data reception if the timeout expires. */
    if (timeout == 0U)
    {
        LPSPI_DRV_SlaveAbortTransfer(BCC_TPL_RX_LPSPI_INSTANCE);
        return BCC_STATUS_COM_TIMEOUT;
    }

    /* Store received data to recvBuf. */
    for (i = 0; i < recvTrCnt; i++)
    {
        recvBuf[BCC_MSG_SIZE * i + 0] = rBuf[LPSPI_ALIGNMENT * i + 3];
        recvBuf[BCC_MSG_SIZE * i + 1] = rBuf[LPSPI_ALIGNMENT * i + 2];
        recvBuf[BCC_MSG_SIZE * i + 2] = rBuf[LPSPI_ALIGNMENT * i + 1];
        recvBuf[BCC_MSG_SIZE * i + 3] = rBuf[LPSPI_ALIGNMENT * i + 0];
        recvBuf[BCC_MSG_SIZE * i + 4] = rBuf[LPSPI_ALIGNMENT * i + 4];
    }

    return BCC_STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_Assert
 * Description   : User implementation of assert.
 *
 *END**************************************************************************/
void BCC_MCU_Assert(bool x)
{
    DEV_ASSERT(x);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_WriteCsbPin
 * Description   : Writes logic 0 or 1 to the CSB pin (or CSB_TX in case of TPL
 *                 mode).
 *
 *END**************************************************************************/
void BCC_MCU_WriteCsbPin(uint8_t drvInstance, uint8_t value)
{
#if defined(SPI)
    LPSPI_DRV_SetPcs(BCC_SPI_LPSPI_INSTANCE, BCC_SPI_LPSPI_PCS,
            value ? LPSPI_ACTIVE_LOW : LPSPI_ACTIVE_HIGH);
#elif defined(TPL)
    LPSPI_DRV_SetPcs(BCC_TPL_TX_LPSPI_INSTANCE, BCC_TPL_TX_LPSPI_PCS,
            value ? LPSPI_ACTIVE_LOW : LPSPI_ACTIVE_HIGH);
#elif defined(TPL_TRANSLT)
    PINS_DRV_WritePin(BCC_TPL_TX_PCS_INSTANCE, BCC_TPL_TX_PCS_PIN, value);
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_WriteRstPin
 * Description   : Writes logic 0 or 1 to the RST pin.
 *
 *END**************************************************************************/
void BCC_MCU_WriteRstPin(uint8_t drvInstance, uint8_t value)
{
#if defined(SPI)
    PINS_DRV_WritePin(BCC_RST_INSTANCE, BCC_RST_INDEX, value);
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_WriteEnPin
 * Description   : Writes logic 0 or 1 to the EN pin of MC33664.
 *
 *END**************************************************************************/
void BCC_MCU_WriteEnPin(uint8_t drvInstance, uint8_t value)
{
#if defined(TPL) || defined(TPL_TRANSLT)
    PINS_DRV_WritePin(BCC_EN_INSTANCE, BCC_EN_INDEX, value);
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : BCC_MCU_WriteEnPin
 * Description   : Reads logic value of INTB pin of MC33664.
 *
 *END**************************************************************************/
uint32_t BCC_MCU_ReadIntbPin(uint8_t drvInstance)
{
#if defined(TPL) || defined(TPL_TRANSLT)
    return (PINS_DRV_ReadPins(BCC_INTB_INSTANCE) >> BCC_INTB_INDEX) & 1;
#elif defined(SPI)
    return 0;
#endif
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
