/**
 *******************************************************************************
 *  @file 	HID.h
 *  @brief 	Steering modul  basic header. All Steering projects need to implement ECU_Main function!
 *  		<b>BME Formula Racing Team - Electronics Team</b>
 *	@date 	2017.07.10.
 *  @author Abraham Zoltan
 *  @version 0.2
 *
 *  <B> Changelog: <BR></B>
 *
 *******************************************************************************
 */

#ifndef STEERING_0_21_FRT_LIBRARY_ECU_LAYER_HID_H_
#define STEERING_0_21_FRT_LIBRARY_ECU_LAYER_HID_H_

#include "stdint.h"

/*-------------------- Defines --------------------*/
#define ROTARY_1		STAMP_ADC2_INDEX
#define ROTARY_2		STAMP_ADC18_INDEX
#define ROTARY_3		STAMP_ADC6_INDEX
#define ROTARY_4		STAMP_ADC14_INDEX
#define ROTARY_5		STAMP_ADC10_INDEX

#define BUTTON_1		STAMP_ADC4_INDEX
#define BUTTON_2		STAMP_ADC16_INDEX
#define BUTTON_3		STAMP_ADC8_INDEX
#define BUTTON_4		STAMP_ADC12_INDEX

#define ButtonN 		4
#define ButtonStateN 	2
#define RotaryN 		5
#define RotaryPosN 		12

#define DigitN			8
#define DISPLAY_SPI		STAMP_SPI1
#define CHIPSELECT		STAMP_GPIO1

#define LedN			16
#define DCdata			7
#define LED_SPI			STAMP_SPI2
#define BLANK 			STAMP_GPIO2
#define XLAT 			STAMP_GPIO3
#define MODE 			STAMP_GPIO4

/*-------------------- Functions --------------------*/
	/* Reading Buttons and Rotaries */
	void getRot(void);			//!< Read rotary switch positions
	void getBut(void);			//!< Read button states

	/* Writing to the display */
	void writeByteToDisplay(uint16_t address, uint8_t byte);
	void testDisplay(void);
	void printDisplay(char data[DigitN], uint8_t point[DigitN], uint8_t plane);
	void initDisplay(void);

	/* Controlling the LEDs */
	void testLED(void);
	void setLEDState(uint16_t redLED, uint16_t greenLED, uint16_t blueLED);
	void setLEDBright(uint8_t redBright[LedN], uint8_t greenBright[LedN], uint8_t blueBright[LedN]);
	void initLED(void);

#endif /* STEERING_0_21_FRT_LIBRARY_ECU_LAYER_HID_H_ */
