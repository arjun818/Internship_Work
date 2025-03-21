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
 ** @file main.c
 ** @version 1.1
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including necessary module. Cpu.h contains other modules needed for compiling.*/
#include "Cpu.h"

volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "utils/nxp_console.h" /* PRINTF */
#include "bcc/bcc.h"
#include "bcc_s32k144/bcc_peripheries.h"
#include "common.h"
#include "monitoring.h"

/**********************************************************/
/****Added by Arjun G****/
#include "pins_driver.h"// Include the GPIO driver or header for your specific platform
#include "clock_manager.h"  // Include delay functions, or define your own
#include "osif.h"
#include "device_registers.h"
#include "devassert.h"

/* Red on-board LED. */
#define RED_LED_PORT   PTD
#define RED_LED_PIN    15U
/*********************************************************/
/****Constants defined by Arjun G****/
#define BUTTON_PIN        				5U  // Replace with the actual GPIO pin where the button is connected
#define BUTTON_PORT						PTC //Not needed

#define LED_PIN_1						10U
#define LED_PIN_2						11U
#define LED_PIN_3						12U
#define LED_PIN_4						13U
#define LED_PORT						PTA//Not needed

#define NUM_OF_LED_PINS      			4
#define NUM_OF_BUTTON_PINS   			1

#define MAX_MEASUREMENTS 				10U
#define DELAY							4U

#define PCC_CLOCKPCC_PORTA_CLOCK
#define PCC_CLOCKPCC_PORTC_CLOCK

#define BUTTON_DEBOUNCE_DELAY  			50U  // Debounce delay in milliseconds

#define SHORT_PRESS_TIME				1000U  //1 second for short press detection (in milliseconds)
#define LONG_PRESS_TIME        			2000U  // 2 seconds for long press detection (in milliseconds)
#define MAX_ON_DELAY					300000U//5 minutes

static uint32_t buttonPressStartTime = 0;
static bool buttonPressed = false;
static uint32_t max_delay = 0;
bcc_data_t g_bccData;
/*******************************************************************************
 * Pin-muxing configuration
 ******************************************************************************/

#if defined(TPL) || defined(TPL_TRANSLT)
    #define NUM_OF_SPECIFIC_PINS      12
#else
#define NUM_OF_SPECIFIC_PINS      9
#endif

/* Pin-muxing configuration related to the specific device board. */
pin_settings_config_t g_pin_mux_specific[NUM_OF_SPECIFIC_PINS] =
		{
				/* MISO/DATA_RX. */
				{ .base = PORTB, .pinPortIdx = 3u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT3, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },
				/* SCLK/SCLK_RX. */
				{ .base = PORTB, .pinPortIdx = 2u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT3, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },

#if defined(TPL) || defined(TPL_TRANSLT)
    /* CSB_RX */
    {
        .base          = PORTB,
        .pinPortIdx    = 5u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_ALT3,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = NULL,
        .digitalFilter = false,
    },
    /* EN */
    {
        .base          = PORTB,
        .pinPortIdx    = 0u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_AS_GPIO,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = PTB,
        .direction     = GPIO_OUTPUT_DIRECTION,
        .digitalFilter = false,
        .initValue     = 0u,
    },
    /* CLK_TX */
    {
        .base          = PORTB,
        .pinPortIdx    = 14u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_ALT3,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = NULL,
        .digitalFilter = false,
    },
    /* DATA_TX */
    {
        .base          = PORTB,
        .pinPortIdx    = 16u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_ALT3,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = NULL,
        .digitalFilter = false,
    },
#endif

#if defined(TPL_TRANSLT)
    /* CSB_TX */
    {
        .base          = PORTE,
        .pinPortIdx    = 9u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_AS_GPIO,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = PTE,
        .direction     = GPIO_OUTPUT_DIRECTION,
        .digitalFilter = false,
        .initValue     = 1u,
    },
    /* INTB */
    {
        .base          = PORTA,
        .pinPortIdx    = 6u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_AS_GPIO,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = PTA,
        .direction     = GPIO_INPUT_DIRECTION,
        .digitalFilter = false,
    },
#elif defined(TPL)
    /* CSB_TX */
    {
        .base          = PORTD,
        .pinPortIdx    = 3u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_ALT3,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = NULL,
        .digitalFilter = false,
    },
    /* INTB */
    {
        .base          = PORTB,
        .pinPortIdx    = 1u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_AS_GPIO,
        .pinLock       = false,
        .intConfig     = PORT_DMA_INT_DISABLED,
        .clearIntFlag  = false,
        .gpioBase      = PTB,
        .direction     = GPIO_INPUT_DIRECTION,
        .digitalFilter = false,
    },
#endif

#if defined(SPI)
				/* CSB. */
				{ .base = PORTB, .pinPortIdx = 5u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT3, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },
				/* RESET. */
				{ .base = PORTD, .pinPortIdx = 4u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_AS_GPIO, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = PTD, .direction = GPIO_OUTPUT_DIRECTION,
						.digitalFilter = false, .initValue = 0u, },
				/* SPI MOSI. */
				{ .base = PORTB, .pinPortIdx = 4u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT3, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },
#endif

#ifdef SPI
				/* FAULT. */
				{ .base = PORTB, .pinPortIdx = 9u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_AS_GPIO, .pinLock = false, .intConfig =
								PORT_INT_RISING_EDGE, .clearIntFlag = true,
						.gpioBase = PTB, .direction = GPIO_INPUT_DIRECTION,
						.digitalFilter = false, },
#else
    /* FAULT - TPL or TPL_TRANSLT */
    {
        .base          = PORTE,
        .pinPortIdx    = 8u,
        .pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilter = false,
        .driveSelect   = PORT_LOW_DRIVE_STRENGTH,
        .mux           = PORT_MUX_AS_GPIO,
        .pinLock       = false,
        .intConfig     = PORT_INT_RISING_EDGE,
        .clearIntFlag  = true,
        .gpioBase      = PTE,
        .direction     = GPIO_INPUT_DIRECTION,
        .digitalFilter = false,
    },
#endif

				/* LPUART TX. */
				{ .base = PORTC, .pinPortIdx = 7u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT2, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },
				/* LPUART RX. */
				{ .base = PORTC, .pinPortIdx = 6u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_ALT2, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = NULL, .digitalFilter = false, },
				/* Red LED. */
				{ .base = PORTD, .pinPortIdx = 15u, .pullConfig =
						PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false,
						.driveSelect = PORT_LOW_DRIVE_STRENGTH, .mux =
								PORT_MUX_AS_GPIO, .pinLock = false, .intConfig =
								PORT_DMA_INT_DISABLED, .clearIntFlag = false,
						.gpioBase = PTD, .direction = GPIO_OUTPUT_DIRECTION,
						.digitalFilter = false, .initValue = 1u, }, };

/*******************************************************************************
 * Initial BCC configuration
 ******************************************************************************/

/* Initial configuration of Battery Cell Controller devices. */
static const uint16_t BCC_INIT_CONF[1][BCC_INIT_CONF_REG_CNT] = { {
BCC_CONF1_GPIO_CFG1_VALUE,
BCC_CONF1_GPIO_CFG2_VALUE,
BCC_CONF1_TH_ALL_CT_VALUE,
#ifdef MC33771
		BCC_CONF1_TH_CT14_VALUE,
		BCC_CONF1_TH_CT13_VALUE,
		BCC_CONF1_TH_CT12_VALUE,
		BCC_CONF1_TH_CT11_VALUE,
		BCC_CONF1_TH_CT10_VALUE,
		BCC_CONF1_TH_CT9_VALUE,
		BCC_CONF1_TH_CT8_VALUE,
		BCC_CONF1_TH_CT7_VALUE,
#else
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
#endif
		BCC_CONF1_TH_CT6_VALUE,
		BCC_CONF1_TH_CT5_VALUE,
		BCC_CONF1_TH_CT4_VALUE,
		BCC_CONF1_TH_CT3_VALUE,
		BCC_CONF1_TH_CT2_VALUE,
		BCC_CONF1_TH_CT1_VALUE,
		BCC_CONF1_TH_AN6_OT_VALUE,
		BCC_CONF1_TH_AN5_OT_VALUE,
		BCC_CONF1_TH_AN4_OT_VALUE,
		BCC_CONF1_TH_AN3_OT_VALUE,
		BCC_CONF1_TH_AN2_OT_VALUE,
		BCC_CONF1_TH_AN1_OT_VALUE,
		BCC_CONF1_TH_AN0_OT_VALUE,
		BCC_CONF1_TH_AN6_UT_VALUE,
		BCC_CONF1_TH_AN5_UT_VALUE,
		BCC_CONF1_TH_AN4_UT_VALUE,
		BCC_CONF1_TH_AN3_UT_VALUE,
		BCC_CONF1_TH_AN2_UT_VALUE,
		BCC_CONF1_TH_AN1_UT_VALUE,
		BCC_CONF1_TH_AN0_UT_VALUE,
		BCC_CONF1_TH_ISENSE_OC_VALUE,
		BCC_CONF1_TH_COULOMB_CNT_MSB_VALUE,
		BCC_CONF1_TH_COULOMB_CNT_LSB_VALUE,
		BCC_CONF1_CB1_CFG_VALUE,
		BCC_CONF1_CB2_CFG_VALUE,
		BCC_CONF1_CB3_CFG_VALUE,
		BCC_CONF1_CB4_CFG_VALUE,
		BCC_CONF1_CB5_CFG_VALUE,
		BCC_CONF1_CB6_CFG_VALUE,
#ifdef MC33771
		BCC_CONF1_CB7_CFG_VALUE,
		BCC_CONF1_CB8_CFG_VALUE,
		BCC_CONF1_CB9_CFG_VALUE,
		BCC_CONF1_CB10_CFG_VALUE,
		BCC_CONF1_CB11_CFG_VALUE,
		BCC_CONF1_CB12_CFG_VALUE,
		BCC_CONF1_CB13_CFG_VALUE,
		BCC_CONF1_CB14_CFG_VALUE,
#else
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
#endif
		BCC_CONF1_OV_UV_EN_VALUE,
		BCC_CONF1_SYS_CFG1_VALUE,
		BCC_CONF1_SYS_CFG2_VALUE,
		BCC_CONF1_ADC_CFG_VALUE,
		BCC_CONF1_ADC2_OFFSET_COMP_VALUE,
		BCC_CONF1_FAULT_MASK1_VALUE,
		BCC_CONF1_FAULT_MASK2_VALUE,
		BCC_CONF1_FAULT_MASK3_VALUE,
		BCC_CONF1_WAKEUP_MASK1_VALUE,
		BCC_CONF1_WAKEUP_MASK2_VALUE,
		BCC_CONF1_WAKEUP_MASK3_VALUE,
		BCC_CONF1_CELL_OV_FLT_VALUE,
		BCC_CONF1_CELL_UV_FLT_VALUE,
		BCC_CONF1_AN_OT_UT_FLT_VALUE,
		BCC_CONF1_CB_SHORT_FLT_VALUE,
		BCC_CONF1_GPIO_STS_VALUE,
		BCC_CONF1_GPIO_SHORT_VALUE,
		BCC_CONF1_FAULT1_STATUS_VALUE,
		BCC_CONF1_FAULT2_STATUS_VALUE,
		BCC_CONF1_FAULT3_STATUS_VALUE, }, };

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/

static void initDemo(status_t *error, bcc_status_t *bccError);

static bcc_status_t startApp(void);

/****Added by Arjun G****/
void init_gpio(void);

void PORTC_IRQHandler(void);

void led_handling_func(void);
/*******************************************************************************
 * Functions
 ******************************************************************************/

/*!
 * @brief FAULT pin IRQ handler.
 */
#ifdef SPI
void PORTB_IRQHandler(void) {
	if (PINS_DRV_GetPortIntFlag(PORTB) & (1 << 9U)) {
		/* Fault pin interrupt. */
		PINS_DRV_ClearPinIntFlagCmd(PORTB, 9U);
	}
}
#else
/* #ifdef TPL or TPL_TRANSLT */
void PORTE_IRQHandler(void)
{
    if (PINS_DRV_GetPortIntFlag(PORTE) & (1 << 8U))
    {
        /* Fault pin interrupt. */
        PINS_DRV_ClearPinIntFlagCmd(PORTE, 8U);
    }
}
#endif

/*!
 * @brief MCU and BCC initialization.
 */
/**************************************************************************/
/****Added by Arjun G****/

/* Pin-muxing configuration related to the specific device board. */
pin_settings_config_t g_pin_mux_led[NUM_OF_LED_PINS] = {
/* LED-1 */
{ .base = PORTA, .pinPortIdx = LED_PIN_1, .pullConfig =
		PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false, .driveSelect =
		PORT_LOW_DRIVE_STRENGTH, .mux = PORT_MUX_AS_GPIO, .pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED, .clearIntFlag = false, .gpioBase =
				PTA, .direction = GPIO_OUTPUT_DIRECTION, .digitalFilter = false,
		.initValue = 1U },
/* LED-2 */
{ .base = PORTA, .pinPortIdx = LED_PIN_2, .pullConfig =
		PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false, .driveSelect =
		PORT_LOW_DRIVE_STRENGTH, .mux = PORT_MUX_AS_GPIO, .pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED, .clearIntFlag = false, .gpioBase =
				PTA, .direction = GPIO_OUTPUT_DIRECTION, .digitalFilter = false,
		.initValue = 1U },
/* LED-3 */
{ .base = PORTA, .pinPortIdx = LED_PIN_3, .pullConfig =
		PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false, .driveSelect =
		PORT_LOW_DRIVE_STRENGTH, .mux = PORT_MUX_AS_GPIO, .pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED, .clearIntFlag = false, .gpioBase =
				PTA, .direction = GPIO_OUTPUT_DIRECTION, .digitalFilter = false,
		.initValue = 1U },
/* LED-4 */
{ .base = PORTA, .pinPortIdx = LED_PIN_4, .pullConfig =
		PORT_INTERNAL_PULL_NOT_ENABLED, .passiveFilter = false, .driveSelect =
		PORT_LOW_DRIVE_STRENGTH, .mux = PORT_MUX_AS_GPIO, .pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED, .clearIntFlag = false, .gpioBase =
				PTA, .direction = GPIO_OUTPUT_DIRECTION, .digitalFilter = false,
		.initValue = 1U } };

pin_settings_config_t g_pin_mux_button[NUM_OF_BUTTON_PINS] = {
/* Button */
{ .base = PORTC, .pinPortIdx = BUTTON_PIN, .pullConfig =
		PORT_INTERNAL_PULL_UP_ENABLED, .passiveFilter = false, .driveSelect =
		PORT_LOW_DRIVE_STRENGTH, .mux = PORT_MUX_AS_GPIO, .pinLock = false,
		.intConfig = PORT_INT_RISING_EDGE, .clearIntFlag = false, .gpioBase =
				PTC, .direction = GPIO_INPUT_DIRECTION, .digitalFilter = false,
		.initValue = 1U } };

/****Added by Arjun*****/

void init_gpio(void) {
	status_t error;

	//1. Configure clocks for all the PORTS
	error = CLOCK_DRV_Init(&clockMan1_InitConfig0);
	DEV_ASSERT(error == STATUS_SUCCESS);

	//2. Configure pins as GPIO for LEDs
	error = PINS_DRV_Init(NUM_OF_LED_PINS, g_pin_mux_led);
	DEV_ASSERT(error == STATUS_SUCCESS);

	//3. Configure pins as GPIO for button
	error = PINS_DRV_Init(NUM_OF_BUTTON_PINS, g_pin_mux_button);
	DEV_ASSERT(error == STATUS_SUCCESS);

	//4. Configure interrupt for the button
	INT_SYS_InstallHandler(PORTC_IRQn, PORTC_IRQHandler, (isr_t*) 0);
	INT_SYS_EnableIRQ(PORTC_IRQn);
}

static void initDemo(status_t *error, bcc_status_t *bccError) {
	ntc_config_t ntcConfig;

	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
			g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);

	/* Pin-muxing + GPIO pin direction and initial value settings. */
	PINS_DRV_Init(NUM_OF_SPECIFIC_PINS, g_pin_mux_specific);

	/* Initialize LPUART instance */
	*error = DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_TYPE,
			&lpuart1_State, &lpuart1_InitConfig0);
	if (*error != STATUS_SUCCESS) {
		return;
	}

	/* Initialize LPSPI instance(s) */
	*error = BCC_MCU_ConfigureLPSPI();
	if (*error != STATUS_SUCCESS) {
		return;
	}

	/* Initialize BCC driver configuration structure (g_bccData.drvConfig). */
	g_bccData.drvConfig.drvInstance = 0U;
	g_bccData.drvConfig.devicesCnt = 1U;
#ifdef MC33771
	g_bccData.drvConfig.device[0] = BCC_DEVICE_MC33771;
	g_bccData.drvConfig.cellCnt[0] = 14U;
	g_bccData.drvConfig.device[1] = BCC_DEVICE_MC33771;
	g_bccData.drvConfig.cellCnt[1] = 14U;
#else
    g_bccData.drvConfig.device[0] = BCC_DEVICE_MC33772;
    g_bccData.drvConfig.cellCnt[0] = 6U;
#endif
#if defined(SPI)
	g_bccData.drvConfig.commMode = BCC_MODE_SPI;
#else
    g_bccData.drvConfig.commMode = BCC_MODE_TPL;
#endif

	/* Precalculate NTC look up table for fast temperature measurement. */
	ntcConfig.rntc = 6800U; /* NTC pull-up 6.8kOhm */
	ntcConfig.refTemp = 25U; /* NTC resistance 10kOhm at 25 degC */
	ntcConfig.refRes = 10000U; /* NTC resistance 10kOhm at 25 degC */
	ntcConfig.beta = 3900U;
	fillNtcTable(&ntcConfig);

	/* FAULT pin: Clear interrupt flag and enable interrupts. */
#ifdef SPI
	INT_SYS_ClearPending(PORTB_IRQn);
	INT_SYS_EnableIRQ(PORTB_IRQn);
#else
    /* #ifdef TPL or TPL_TRANSLT */
    INT_SYS_ClearPending(PORTE_IRQn);
    INT_SYS_EnableIRQ(PORTE_IRQn);
#endif

	/* Initialize BCC device */
	*bccError = BCC_Init(&g_bccData.drvConfig, BCC_INIT_CONF);
}
/*************************************************************/
/****Added by Arjun G****/
void led_handling_func(void) {
	static bool ledState = false;

	// Check if the button is pressed (assuming active low)
	if (PINS_DRV_ReadPins(BUTTON_PORT) == 0) {
		if (!buttonPressed) {
			// Button was just pressed, start the timer
			buttonPressStartTime = OSIF_GetMilliseconds(); // Function to get current time in ms
			buttonPressed = true;
		}
		/*******************1. For handling 'ON' state of battery*****************************/
		if (buttonPressed) {
			// Check if the button has been pressed for a long time
			if ((OSIF_GetMilliseconds() - buttonPressStartTime
					>= LONG_PRESS_TIME)
					&& (OSIF_GetMilliseconds() - buttonPressStartTime
							<= SHORT_PRESS_TIME)) {
				// Long press detected
				PRINTF("Long press detected\r\n");
				// Perform any action you want for long press, e.g., reset the system or change a mode
				uint16_t sum = (BCC_GET_VOLT(BCC_MSR_CELL_VOLT1)
						+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT2)
						+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT3)
						+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT4)
						+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT5)
						+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT6)) / (double) 1000000;

				// Perform actions based on voltage levels
				while (max_delay <= MAX_ON_DELAY) {

					if (sum >= 18.9 && sum <= 25.2) {
						PINS_DRV_SetPins(LED_PORT,
								LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
						ledState = true;
					} else if (sum >= 12.6 && sum <= 18.9) {
						for (;;) {
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
							BCC_MCU_WaitMs(100);
							PINS_DRV_SetPins(LED_PORT, LED_PIN_1);
							BCC_MCU_WaitMs(100);
						}
						PINS_DRV_SetPins(LED_PORT,
								LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
						ledState = true;
					} else if (sum >= 6.3 && sum <= 12.6) {
						for (;;) {
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
							BCC_MCU_WaitMs(100);
							PINS_DRV_SetPins(LED_PORT, LED_PIN_1 | LED_PIN_2);
							BCC_MCU_WaitMs(100);
						}
						ledState = true;
						PINS_DRV_SetPins(LED_PORT, LED_PIN_3 | LED_PIN_4);
					} else {
						for (;;) {
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
							PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
							BCC_MCU_WaitMs(100);
							PINS_DRV_SetPins(LED_PORT,
									LED_PIN_1 | LED_PIN_2 | LED_PIN_3);
							BCC_MCU_WaitMs(100);
						}
						PINS_DRV_SetPins(LED_PORT, LED_PIN_4);
						ledState = true; //Set Led flag
					}
					max_delay++;
				}
				ledState = false;
				buttonPressed = false;  // Reset buttonPressed flag
			}
		}
		/***********************2. For handling 'STATUS' state of battery****************************/
		else if (buttonPressed) {
			uint16_t sum = (BCC_GET_VOLT(BCC_MSR_CELL_VOLT1)
					+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT2)
					+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT3)
					+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT4)
					+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT5)
					+ BCC_GET_VOLT(BCC_MSR_CELL_VOLT6)) / (double) 1000000;
			if (OSIF_GetMilliseconds()
					- buttonPressStartTime<= SHORT_PRESS_TIME) {
				// Short press detected
				PRINTF("Short press detected\r\n");
				if (sum >= 18.9 && sum <= 25.2) {
					PINS_DRV_SetPins(LED_PORT,
							LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
					//Delay of 4s
					BCC_MCU_WaitSec(DELAY);

					//Then switch off the LED
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_4);
				} else if (sum >= 12.6 && sum <= 18.9) {

					for (int i = 0; i < DELAY; i++) {
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
						BCC_MCU_WaitMs(100);
						PINS_DRV_SetPins(LED_PORT, LED_PIN_1);
						BCC_MCU_WaitMs(100);
					}

					PINS_DRV_SetPins(LED_PORT,
							LED_PIN_2 | LED_PIN_3 | LED_PIN_4);
					//Delay of 4s
					BCC_MCU_WaitSec(DELAY);

					//Then switch off the LED
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_4);

				} else if (sum >= 6.3 && sum <= 12.6) {
					for (int i = 0; i < DELAY; i++) {
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
						BCC_MCU_WaitMs(100);
						PINS_DRV_SetPins(LED_PORT, LED_PIN_1 | LED_PIN_2);
						BCC_MCU_WaitMs(100);
					}
					PINS_DRV_SetPins(LED_PORT, LED_PIN_3);
					PINS_DRV_SetPins(LED_PORT, LED_PIN_4);

					//Delay of 4s
					BCC_MCU_WaitSec(DELAY);
					//Then switch off the LED

					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_4);
				} else {
					for (int i = 0; i < DELAY; i++) {
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
						PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
						BCC_MCU_WaitMs(100);
						PINS_DRV_SetPins(LED_PORT,
								LED_PIN_1 | LED_PIN_2 | LED_PIN_3);
						BCC_MCU_WaitMs(100);
					}
					PINS_DRV_SetPins(LED_PORT, LED_PIN_4);
					//Delay of 4s
					BCC_MCU_WaitSec(DELAY);

					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
					PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_4);
				}
			}
			ledState = false;
			buttonPressed = false;  // Reset buttonPressed flag
		}
		/************************3. For handling 'OFF' state of battery***********************/
		else {
			if ((OSIF_GetMilliseconds() - buttonPressStartTime
					>= LONG_PRESS_TIME)
					&& (OSIF_GetMilliseconds() - buttonPressStartTime
							<= SHORT_PRESS_TIME) && (ledState == true)) {
				PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_1);
				PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_2);
				PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_3);
				PINS_DRV_ClearPins(LED_PORT, 1U << LED_PIN_4);
			}
			ledState = false;
			buttonPressed = false;
		}
		// Optionally de-bounce the button (i.e., ignore further presses for a short period)
		BCC_MCU_WaitMs(BUTTON_DEBOUNCE_DELAY);
		max_delay = 0;
	}  //End of Main 'if' statement
}

/****Added by Arjun G****/
/************Soft start method***************/
void PORTC_IRQHandler(void) {
	led_handling_func();
}

/* Dummy functions for time and GPIO handling for illustration */

/*!
 * @brief This function executes all prepared test cases.
 *
 * @return bcc_status_t Error code.
 */
static bcc_status_t startApp(void) {
	uint8_t cid;
	bcc_status_t error;

	for (cid = BCC_CID_DEV1; cid <= g_bccData.drvConfig.devicesCnt; cid++) {
		if ((error = printInitialSettings(cid)) != BCC_STATUS_SUCCESS) {
			return error;
		}

		if ((error = doMeasurements(cid)) != BCC_STATUS_SUCCESS) {
			return error;
		}

		if ((error = printFaultRegisters(cid)) != BCC_STATUS_SUCCESS) {
			return error;
		}
	}

	return BCC_STATUS_SUCCESS;
}

/*!
 \brief The main function for the project.
 \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
 */
int main(void) {
	/* Write your local variable definition here */
	status_t error;
	bcc_status_t bccError;

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
#ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
	/*** End of Processor Expert internal initialization.                    ***/

	initDemo(&error, &bccError);
	if (error != STATUS_SUCCESS) {
		PRINTF(
				"An error occurred during initialization of MCU peripherals.\r\n");
		PINS_DRV_ClearPins(RED_LED_PORT, 1U << RED_LED_PIN);
	} else if (bccError != BCC_STATUS_SUCCESS) {
		PRINTF("An error occurred during BCC initialization: (0x%04x)\r\n",
				bccError);
		PINS_DRV_ClearPins(RED_LED_PORT, 1U << RED_LED_PIN);
	} else {
		PRINTF("------------- BEGIN ---------------\r\n");

		if ((bccError = startApp()) != BCC_STATUS_SUCCESS) {
			PRINTF("An error occurred (0x%04x)\r\n)", bccError);
			PINS_DRV_ClearPins(RED_LED_PORT, 1U << RED_LED_PIN);
		}

		bccError = BCC_Sleep(&g_bccData.drvConfig);
		if (bccError != BCC_STATUS_SUCCESS) {
			PRINTF("SLEEP (0x%04x)\r\n)", bccError);
			PINS_DRV_ClearPins(RED_LED_PORT, 1U << RED_LED_PIN);
		}

		PRINTF("-------------- END ----------------\r\n");
	}
	/****Added by Arjun G****/
	//1. Add the GPIO initialization below
	init_gpio();

	//2. Add the cell balancing API to enable cell balancing
	bcc_drv_config_t myConfig;
	bccError = BCC_CB_Enable(&myConfig, BCC_CID_DEV2, true);
	DEV_ASSERT(bccError == BCC_STATUS_SUCCESS);

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for (;;) {
		if (exit_code != 0) {
			break;
		}
	}
	return exit_code;
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
}

/*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.1 [05.21]
 **     for the NXP S32K series of microcontrollers.
 **
 ** ###################################################################
 */
