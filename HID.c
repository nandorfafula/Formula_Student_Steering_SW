/**
 *******************************************************************************
 *  @file 	HID.c
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

/* Includes */
#include "HID.h"
#include "Steering.h"

/*-------------------- Variables --------------------*/
/* Buttons and Rotaries */
	uint8_t ButtonStates[ButtonN]; 						//Output array of buttons
	const static uint16_t ButStateADC[ButtonStateN] = {2048, 4095}; 	//ADC values of button states
	const static uint8_t ButStateActual[ButtonStateN] = {0, 1}; 		//Output values of button states
	uint8_t RotaryPositions[RotaryN];	//Output array of rotary switches
	/* ADC values of rotary positions, ordered pos1,pos2,... taken from Steering_ECU_0.11 */
	const static uint16_t RotPosADC[RotaryPosN] = {0, 100, 300, 700, 1100, 1500, 1800, 2200, 2600, 3000, 3300, 3700};
	/* Output values of rotary positions matched to RotPosADC taken from Steering_ECU_0.11 */
	const static uint8_t RotPosActual[RotaryPosN] = {7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6};
/* Display */
	uint16_t address;
	uint8_t byte;
	char data[DigitN];
	uint8_t point[DigitN];
	uint8_t plane;
/* LEDs */
	uint16_t redLED;
	uint16_t greenLED;
	uint16_t blueLED;
	uint8_t redBright[LedN];
	uint8_t greenBright[LedN];
	uint8_t blueBright[LedN];

/* Reading Rotary switch positions */
void getRot(void)
{
	/* ADC temp */
	uint16_t RotTemp[RotaryN];
	/* ADC read */
	RotTemp[0] = STAMP_ADC_getValue(ROTARY_1);
	RotTemp[1] = STAMP_ADC_getValue(ROTARY_2);
	RotTemp[2] = STAMP_ADC_getValue(ROTARY_3);
	RotTemp[3] = STAMP_ADC_getValue(ROTARY_4);
	RotTemp[4] = STAMP_ADC_getValue(ROTARY_5);

	/* Convert ADC values to Rotary Positions and write to array */
	uint8_t i = 0u;
	uint8_t j = 0u;
	for(i = 0; i < RotaryN; i++)
	{
		for(j = 0; j < RotaryPosN; j++)
		{
			if(RotTemp[i] > RotPosADC[j])
			{
				RotaryPositions[i] = RotPosActual[j];
			}
		}
	}
}

void getBut(void)
{
	/* ADC temp */
	uint16_t ButTemp[ButtonN] = {0,0,0,0};
	/* ADC read */
	ButTemp[0] = STAMP_ADC_getValue(BUTTON_1);
	ButTemp[1] = STAMP_ADC_getValue(BUTTON_2);
	ButTemp[2] = STAMP_ADC_getValue(BUTTON_3);
	ButTemp[3] = STAMP_ADC_getValue(BUTTON_4);

	/* Convert ADC values to Button States and write to array */
	uint8_t i = 0u;
	for(i = 0; i < ButtonN; i++)
	{
		if(ButTemp[i] < ButStateADC[0])
			ButtonStates[i] = ButStateActual[0];
		else
			ButtonStates[i] = ButStateActual[1];
	}

}

void writeByteToDisplay(uint16_t address, uint8_t byte)
{
	/* Writes byte to address */
	STAMP_GPIO_ResetBit(CHIPSELECT);
	STAMP_SPI_SendData(DISPLAY_SPI, address);
	STAMP_SPI_SendData(DISPLAY_SPI, byte);
	STAMP_GPIO_SetBit(CHIPSELECT);
}

void testDisplay(void)
{
/* writing registers */
	/* Display test mode */
	writeByteToDisplay(0x07,0x01);
	writeByteToDisplay(0x07,0x00);
	/* Display test mode off */

	/* Digit type - all 8 are 14 segment */
	writeByteToDisplay(0x0C,0xFF);

	/* Control register */
	/* P I R T E B X S
	 * 0/1
	 * S = 1 - Shutdown / Normal operation
	 * X = 1 - ---
	 * B = 1 - Slow blinking 1s / Fast blinking 0.5s
	 * E = 1 - Disable blinking / Enable blinking
	 * T = 0 - Don't reset blink timer on CS rise / Reset blink timer on CS rise
	 * R = 0 - Don't reset P0, P1 plane on CS rise / Reset P0, P1 plane on CS rise
	 * I = 0 - Global brightness / Individual brightness
	 * P = 0 - Blink phase P1 / Blink phase P0
	 * */
	writeByteToDisplay(0x04,0x0F);

	/* Write 'TESTING_' on display plane P0
	 * 0x20 - 0x2F P0
	 * 0x40 - 0x4F P1
	 * 0x60 - 0x6F both planes
	 * */
	writeByteToDisplay(0x20,0x54); // T
	writeByteToDisplay(0x21,0x45); // E
	writeByteToDisplay(0x22,0x53); // S
	writeByteToDisplay(0x23,0x54); // T
	writeByteToDisplay(0x24,0x49); // I
	writeByteToDisplay(0x25,0x4E); // N
	writeByteToDisplay(0x26,0x47); // G
	writeByteToDisplay(0x27,0x5F); // _

	/* Write T.E.S.T.I.N.G._. on display P1 */
	writeByteToDisplay(0x40,0xD4); // T.
	writeByteToDisplay(0x41,0xC5); // E.
	writeByteToDisplay(0x42,0xD3); // S.
	writeByteToDisplay(0x43,0xD4); // T.
	writeByteToDisplay(0x44,0xC9); // I.
	writeByteToDisplay(0x45,0xCE); // N.
	writeByteToDisplay(0x46,0xC7); // G.
	writeByteToDisplay(0x47,0xDF); // _.

	/* Testing brightness */
	uint8_t i = 0x00u;
	for(i = 0x00; i < 0x0F; i++ )
	{
		writeByteToDisplay(0x02,i);
	}

	for(i = 0x0F; i > 0x00; i-- )
	{
		writeByteToDisplay(0x02,i);
	}
}

void printDisplay(char data[DigitN], uint8_t point[DigitN], uint8_t plane)
{
	uint8_t character[DigitN];
	uint8_t i = 0u;
	uint8_t address[DigitN];
	for(i = 0; i < DigitN; i++)
	{
		/* Converting characters to hexadecimal numbers
		 * ASCII characters from 0x20 to 0x7E
		 * Lowercase letters not available with this display
		 * */
		character[i] = data[i];
	}
	/* Adding the dot to the first bit */
	for(i = 0; i < DigitN; i++)
	{
		if (point[i] > 0)
		{
			character[i] += 0x80;
		}
	}

	/* Setting the address to the selected plane
	 * 0 - plane 0
	 * 1 - plane 1
	 * 2 - both planes
	 *  */
	switch(plane)
	{
		case 0:
			for(i = 0; i < DigitN; i++)
			{
				address[i] = 0x20 + i;
			}
			break;
		case 1:
			for(i = 0; i < DigitN; i++)
			{
				address[i] = 0x40 + i;
			}
			break;
		case 2:
			for(i = 0; i < DigitN; i++)
			{
				address[i] = 0x40 + i;
			}
			break;
	}
	/* Writing the data to the display */
	for(i = 0; i < DigitN; i++)
	{
		writeByteToDisplay(address[i], character[i]);
	}
}

void initDisplay(void)
{
	/* Initializing display - read the datasheet before changing */
	writeByteToDisplay(0x01,0xFF); // Decode mode - 8 x 14 segment display
	writeByteToDisplay(0x02,0x0F); // Global intensity - 16/16 - may change if too bright
	writeByteToDisplay(0x04,0x01); // Control register - read testDisplay function for details
	char data[DigitN] = {'F','R','E','C','-','0','0','7'};
	uint8_t point[DigitN] = {0,0,0,0,0,0,0,0};
	printDisplay(data, point, 0);
}

void setLEDState(uint16_t redLED, uint16_t greenLED, uint16_t blueLED)
{
	/* Setting up to send data */
	STAMP_GPIO_ResetBit(MODE); 		// on/off mode
	STAMP_GPIO_ResetBit(XLAT); 		// data latch low
	STAMP_GPIO_ResetBit(BLANK); 	// outputs enabled

	/* Sending data to the led controllers in reverse order
	 * STAMP -> RED_SIN -> RED_SOUT -> GREEN_SIN -> GREEN_SOUT -> BLUE_SIN -> BLUE_SOUT
	 * */
	STAMP_SPI_SendData(LED_SPI, blueLED);
	STAMP_SPI_SendData(LED_SPI, greenLED);
	STAMP_SPI_SendData(LED_SPI, redLED);

	/* Finalizing */
	STAMP_GPIO_SetBit(BLANK); 		// disabling output before change
	STAMP_GPIO_SetBit(XLAT); 		// latches get new data on XLAT rising edge
	STAMP_GPIO_ResetBit(BLANK); 	// enabling output after change
	STAMP_GPIO_ResetBit(XLAT);	 	// pulling XLAT low to end data transfer

}

void setLEDBright(uint8_t redBright[LedN], uint8_t greenBright[LedN], uint8_t blueBright[LedN])
{
	/* Setting up to send data */
	STAMP_GPIO_SetBit(MODE);		// dot correction mode
	STAMP_GPIO_ResetBit(XLAT);		//data latch low
	STAMP_GPIO_ResetBit(BLANK);		// outputs enabled

	/* Sending data to the led controllers in reverse order
	 * STAMP -> RED_SIN -> RED_SOUT -> GREEN_SIN -> GREEN_SOUT -> BLUE_SIN -> BLUE_SOUT
	 * Sending 16(number of leds) * 3(rgb leds) * 7(size of the dot correction register)
	 * 336 bits of data */

	uint8_t i = 0u;
	uint16_t outByte[DCdata]; // output array
	for(i = 0; i < DCdata; i++)
	{
		outByte[i] = 0x0000; // clearing array, otherwise writing the array later would be more complex
	}
	static uint8_t limit = 127u;
	for(i = 0; i < LedN; i++) // check to ensure that every number has a maximum of 7 bits of data
	{
		if(blueBright[i] > limit)
			blueBright[i] = limit;
		if(greenBright[i] > limit)
			greenBright[i] = limit;
		if(redBright[i] > limit)
			redBright[i] = limit;
	}

	/* Sending on SPI
	for(i = 0; i < DCdata; i++)
	{
		outByte[i] = blueBright[i] << (DCdata * i); // shifting the data by i*7 to discard every 8th bit
	}
	STAMP_SPI_Send_Int(LED_SPI, DCdata, outByte); // Sending 112 bits of data

	for(i = 0; i < DCdata; i++)
	{
		outByte[i] = greenBright[i] << (DCdata * i);
	}
	STAMP_SPI_Send_Int(LED_SPI, DCdata, outByte);

	for(i = 0; i < DCdata; i++)
	{
		outByte[i] = redBright[i] << (DCdata * i);
	}
	STAMP_SPI_Send_Int(LED_SPI, DCdata, outByte);
	 */

	/* Writing the output array as 16 * 7 bits of data, bitshifting all the 8th bits away */
	outByte[0] = blueBright[2] >> 5u | blueBright[1] << 2u | blueBright[0] << 9u;
	outByte[1] = blueBright[4] >> 3u | blueBright[3] << 4u | blueBright[2] << 11u;
	outByte[2] = blueBright[6] >> 1u | blueBright[5] << 6u | blueBright[4] << 13u;
	outByte[3] = blueBright[9] >> 6u | blueBright[8] << 1u | blueBright[7] << 8u | blueBright[6] << 15u;
	outByte[4] = blueBright[11] >> 4u | blueBright[10] << 3u | blueBright[9] << 10u;
	outByte[5] = blueBright[13] >> 2u | blueBright[12] << 5u | blueBright[11] << 12u;
	outByte[6] = blueBright[15] | blueBright[14] << 7u | blueBright[13] << 14u;

	STAMP_SPI_Send_Int(LED_SPI,7,outByte); // sending the output array to the led driver

	outByte[0] = greenBright[2] >> 5u | greenBright[1] << 2u | greenBright[0] << 9u;
	outByte[1] = greenBright[4] >> 3u | greenBright[3] << 4u | greenBright[2] << 11u;
	outByte[2] = greenBright[6] >> 1u | greenBright[5] << 6u | greenBright[4] << 13u;
	outByte[3] = greenBright[9] >> 6u | greenBright[8] << 1u | greenBright[7] << 8u | greenBright[6] << 15u;
	outByte[4] = greenBright[11] >> 4u | greenBright[10] << 3u | greenBright[9] << 10u;
	outByte[5] = greenBright[13] >> 2u | greenBright[12] << 5u | greenBright[11] << 12u;
	outByte[6] = greenBright[15] | greenBright[14] << 7u | greenBright[13] << 14u;

	STAMP_SPI_Send_Int(LED_SPI,7,outByte);

	outByte[0] = redBright[2] >> 5u | redBright[1] << 2u | redBright[0] << 9u;
	outByte[1] = redBright[4] >> 3u | redBright[3] << 4u | redBright[2] << 11u;
	outByte[2] = redBright[6] >> 1u | redBright[5] << 6u | redBright[4] << 13u;
	outByte[3] = redBright[9] >> 6u | redBright[8] << 1u | redBright[7] << 8u | redBright[6] << 15u;
	outByte[4] = redBright[11] >> 4u | redBright[10] << 3u | redBright[9] << 10u;
	outByte[5] = redBright[13] >> 2u | redBright[12] << 5u | redBright[11] << 12u;
	outByte[6] = redBright[15] | redBright[14] << 7u | redBright[13] << 14u;

	STAMP_SPI_Send_Int(LED_SPI,7,outByte);

	STAMP_GPIO_SetBit(BLANK); 		// disabling output before change
	STAMP_GPIO_SetBit(XLAT); 		// latches get new data on XLAT rising edge
	STAMP_GPIO_ResetBit(BLANK); 	//enabling output after change
	STAMP_GPIO_ResetBit(XLAT); 		// pulling XLAT low to end data transfer
	STAMP_GPIO_ResetBit(MODE); 		//setting mode back to on/off mode
}

void SoCLED(uint8_t charge) /* Simplified function to drive the top row LEDs */
{

}

void testLED(void)
{
	redLED = greenLED = blueLED = 0u;
}

void initLED(void)
{
 /* Needs brightness */
}








