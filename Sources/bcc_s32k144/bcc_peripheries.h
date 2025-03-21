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

/*!
 * File: bcc_peripheries.h
 *
 * This file implements functions for LPSPI and GPIO operations required by BCC
 * driver. It is closely related to this demo example.
 */

#ifndef BCC_PERIPHERIES_H_
#define BCC_PERIPHERIES_H_

#include "Cpu.h"
#include "bcc_wait.h"
#include "../bcc/bcc.h"
#include "../common.h" /* SPI, TPL or TPL_TRANSLT macro. */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @addtogroup macro_group
 * @{
 */

/*! @brief Timeout used for SPI/TPL communication. */
#define BCC_COM_TIMEOUT_MS        10

/* EN - PTB0 (TPL only) */
#define BCC_EN_INSTANCE           PTB
#define BCC_EN_INDEX              0

#if defined(TPL_TRANSLT)
/* INTB - PTA6 (TPL only) */
#define BCC_INTB_INSTANCE         PTA
#define BCC_INTB_INDEX            6
#elif defined(TPL)
/* INTB - PTB1 (TPL only) */
#define BCC_INTB_INSTANCE         PTB
#define BCC_INTB_INDEX            1
#endif

/* RST - PTD4 (SPI only) */
#define BCC_RST_INSTANCE          PTD
#define BCC_RST_INDEX             4

/* LPSPI configuration (SPI only). */
#define BCC_SPI_LPSPI_INSTANCE    0 /* LPSPI0 */
#define BCC_SPI_LPSPI_BAUD        1000000
#define BCC_SPI_LPSPI_SRCCLK      lpspiCom0_MasterConfig0.lpspiSrcClk
#define BCC_SPI_LPSPI_PCS         LPSPI_PCS1 /* CSB - PTB5/LPSPI0_PCS1 */

/* LPSPI_TX configuration (TPL only). */
#define BCC_TPL_TX_LPSPI_INSTANCE 1 /* LPSPI1 */
#define BCC_TPL_TX_LPSPI_BAUD     2000000
#define BCC_TPL_TX_LPSPI_SRCCLK   lpspiCom1_MasterConfig0.lpspiSrcClk

#if defined(TPL_TRANSLT)
#define BCC_TPL_TX_PCS_INSTANCE   PTE
#define BCC_TPL_TX_PCS_PIN        9 /* CSB_TX - PTE9 */
#elif defined(TPL)
#define BCC_TPL_TX_LPSPI_PCS      LPSPI_PCS0  /* CSB_TX - PTD3 (LPSPI1_PCS0) */
#define BCC_TPL_TX_LPSPI_DELAY_PCS_TO_SCLK          (4u-1u)  /* RM 664 f >= 1.75us */
#define BCC_TPL_TX_LPSPI_DELAY_SCLK_TO_PCS          (2u-1u)  /* RM 664 f >= 0.60us */
#define BCC_TPL_TX_LPSPI_DELAY_BETWEEN_TRANSFERS    (2u-2u)  /* RM 664 h >= 1 us */
#endif

/* LPSPI_RX configuration (TPL only). */
#define BCC_TPL_RX_LPSPI_INSTANCE 0 /* LPSPI0 */
#define BCC_TPL_RX_LPSPI_PCS      LPSPI_PCS1 /* CSB_RX - PTB5/LPSPI0_PCS1 */

/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @addtogroup function_group
 * @{
 */

/*!
 * @brief This function configures LPSPI for usage with BCC driver.
 */
status_t BCC_MCU_ConfigureLPSPI();

 /*!
  * @brief This function performs one 40b transfer via SPI bus. Intended for SPI
  * mode only. This function needs to be implemented for specified MCU by the
  * user.
  *
  * The byte order of buffers is given by BCC_MSG_BIGEND macro (in bcc.h).
  *
  * @param drvInstance Instance of BCC driver.
  * @param transBuf Pointer to 40b data buffer to be sent.
  * @param recvBuf Pointer to 40b data buffer for received data.
  *
  * @return bcc_status_t Error code.
  */
bcc_status_t BCC_MCU_TransferSpi(uint8_t drvInstance, uint8_t transBuf[],
         uint8_t recvBuf[]);

/*!
 * @brief This function sends and receives data via TX and RX SPI buses.
 * Intended for TPL mode only. This function needs to be implemented for
 * specified MCU by the user.
 *
 * TX SPI bus always performs only one 40b SPI transfer. Expected number of RX
 * transfers is passed as a parameter. The byte order of buffers is given by
 * BCC_MSG_BIGEND macro (in bcc.h).
 *
 * @param drvInstance Instance of BCC driver.
 * @param transBuf Pointer to 40b data buffer to be sent.
 * @param recvBuf Pointer to buffer for received data. Its size must be at least
 *                (5 * recvTrCnt) bytes.
 * @param recvTrCnt Number of 40b transfers to be received.
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_MCU_TransferTpl(uint8_t drvInstance, uint8_t transBuf[],
        uint8_t recvBuf[], uint16_t recvTrCnt);

/*!
 * @brief User implementation of assert.
 *
 * @param x - True if everything is OK.
 */
void BCC_MCU_Assert(bool x);

/*!
 * @brief Writes logic 0 or 1 to the CSB pin (or CSB_TX in case of TPL mode).
 * This function needs to be implemented by the user.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value - Zero or one to be set to CSB (CSB_TX) pin.
 */
void BCC_MCU_WriteCsbPin(uint8_t drvInstance, uint8_t value);

/*!
 * @brief Writes logic 0 or 1 to the RST pin.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value - Zero or one to be set to RST pin.
 */
void BCC_MCU_WriteRstPin(uint8_t drvInstance, uint8_t value);

/*!
 * @brief Writes logic 0 or 1 to the EN pin of MC33664.
 *
 * @param drvInstance Instance of BCC driver.
 * @param value - Zero or one to be set to EN pin.
 */
void BCC_MCU_WriteEnPin(uint8_t drvInstance, uint8_t value);

/*!
 * @brief Reads logic value of INTB pin of MC33664.
 *
 * @param drvInstance Instance of BCC driver.
 *
 * @return Zero value for logic zero, non-zero value otherwise.
 */
uint32_t BCC_MCU_ReadIntbPin(uint8_t drvInstance);
/*! @} */

#endif /* BCC_PERIPHERIES_H_ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
