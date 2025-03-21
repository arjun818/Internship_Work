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
 * File: bcc_wait.h
 *
 * This file implements functions for busy waiting for BCC driver.
 */


#ifndef BCC_WAIT_H_
#define BCC_WAIT_H_

#include "device_registers.h"
#include "clock_manager.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @addtogroup macro_group
 * @{
 */
/*!< Gets needed cycles for specified delay in milliseconds, calculation is based on core clock frequency. */
#define BCC_GET_CYCLES_FOR_MS(ms, freq) (((freq) / 1000U) * (ms))

/*!< Gets needed cycles for specified delay in microseconds, calculation is based on core clock frequency. */
#define BCC_GET_CYCLES_FOR_US(us, freq) (((freq) / 1000U) * (us) / 1000U)
/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @addtogroup function_group
 * @{
 */
/*!
 * @brief Returns SCG system clock frequency.
 *
 * @return SCG system clock frequency.
 */
uint32_t BCC_MCU_GetSystemClockFreq(void);

/*!
 * @brief Waits for specified amount of cycles which is given by 32bit
 *        value range. Assumption for this function is that target
 *        architecture is using 32bit general purpose registers.
 *
 * @param cycles - Number of cycles to wait.
 */
void BCC_MCU_WaitCycles(uint32_t cycles);

/*!
 * @brief Waits for specified amount of seconds.
 *
 * @param delay - Number of seconds to wait.
 */
void BCC_MCU_WaitSec(uint16_t delay);

/*!
 * @brief Waits for specified amount of milliseconds.
 *
 * @param delay - Number of milliseconds to wait.
 */
void BCC_MCU_WaitMs(uint16_t delay);

/*!
 * @brief Waits for specified amount of microseconds.
 *
 * @param delay - Number of microseconds to wait.
 */
void BCC_MCU_WaitUs(uint32_t delay);
/*! @} */

#endif /* BCC_WAIT_H_ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
