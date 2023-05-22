/*
 * LedModule.c
 *
 *  Created on: 2 Feb 2020
 *      Author: kitty
 */
#define STM32F407xx
#include "stm32f4xx.h"
#include <stdbool.h>
#include "LedModule.h"

const uint8_t Alphabet[26][5] = { { 0b010, 0b101, 0b111, 0b101, 0b101 },/*A*/
{ 0b110, 0b101, 0b110, 0b101, 0b110 },/*B*/
{ 0b010, 0b101, 0b100, 0b101, 0b010 },/*C*/
{ 0b110, 0b101, 0b101, 0b101, 0b110 },/*D*/
{ 0b111, 0b100, 0b111, 0b100, 0b111 } /*E*/
};
void CLOCK_init() {

	// Enable HSI with HSION bit in RCC_CR

	RCC->CR |= 1;

	// Look if HSI clock is ready? (HSIRDY in RCC_CR is 1)

	while (!(RCC->CR >> 1 & 1)) {
	};

	// Configure the RCC_CFGR APB1  bit fields to set prescalers for APB1 (and maybe AHB1 buses)
	//Prescaler: 4

	RCC->CFGR |= (1 << 12) | (1 << 10);
	RCC->CFGR &= ~(1 << 11);

}

void SPI_init() {

	//Enable SPI Clock by configuring RCC_APB1ENR

	RCC->APB1ENR |= (1 << 14);

	//Configure  PB13, PB15  to AF5 mode to behave as  SPI_CLK, SPI_MOSI  signal

	//a ) Enable the peripheral clock for GPIOB peripheral

	RCC->AHB1ENR |= (1 << 1);

	//b ) Configure the mode of GPIOB pins as alternate function mode

	GPIOB->MODER |= (1 << 24) | (1 << 27) | (1 << 31);
	GPIOB->MODER &= ~(1 << 25) & ~(1 << 26) & ~(1 << 30);

	// GPIOB.12 set high for NSS signal

	GPIOB->ODR |= (1 << 12);

	//c ) Configure the alternation function register to set the mode 5 for  13,15

	GPIOB->AFR[1] |= (1 << 20) | (1 << 22) | (1 << 28) | (1 << 30);

	GPIOB->AFR[1] &= ~(1 << 21) & ~(1 << 23) & ~(1 << 29) & ~(1 << 31);

	// Configure SPI Baud Rate : SPI_CRI register
	// Prescaler 2

	SPI2->CR1 &= ~(0b111 << 3);

	//Select the CPOL and CPHA bits

	SPI2->CR1 &= ~(1 << 1) & ~(1 << 0);

	//  Select the length of data frame : 16-bit

	SPI2->CR1 |= (1 << 11);

	// LSBFirst = 0, because MSB should be sent first.

	SPI2->CR1 &= ~(1 << 7);

	//choose the master configuration

	SPI2->CR1 |= (1 << 2);

	SPI2->CR1 |= (1 << 9);

	SPI2->CR1 |= (1 << 8);

	//enable SPI

	SPI2->CR1 |= (1 << 6);

}

void shutdown(bool onOff) {

	if (onOff == true) {
		spiTransmit(CODE_SHUTDOWN, 0);
	} else {
		spiTransmit(CODE_SHUTDOWN, 1);
	}

}

void clearDisplay(uint8_t *savedDisplayData) {
	for (int i = 0; i < 8; i++) {
		spiTransmit(i + 1, 0);
		*(savedDisplayData + i) = 0;
	}
}

void all_pixels_on(uint8_t *savedDisplayData) {
	for (int i = 0; i < 8; i++) {
		spiTransmit(i + 1, 255);
		*(savedDisplayData + i) = 255;
	}
}

void pixels_on(uint8_t row, uint8_t column, uint8_t *savedDisplayData) {
	column--; // register from 0 to 7
	row--;	//rows are defined between 0 and 7

	uint8_t code;
	uint8_t data = savedDisplayData[row] | (1 << (7 - column));
	*(savedDisplayData + row) = data;

	switch (row) {
	case 0:
		code = DIGIT_0;
		break;
	case 1:
		code = DIGIT_1;
		break;
	case 2:
		code = DIGIT_2;
		break;
	case 3:
		code = DIGIT_3;
		break;
	case 4:
		code = DIGIT_4;
		break;
	case 5:
		code = DIGIT_5;
		break;
	case 6:
		code = DIGIT_6;
		break;
	case 7:
		code = DIGIT_7;
		break;
	}

	spiTransmit(code, data);

}

void pixels_off(uint8_t row, uint8_t column, uint8_t *savedDisplayData) {
	column--;
	row--;

	uint8_t code;
	uint8_t data = savedDisplayData[row] & ~(1 << (7 - column));
	*(savedDisplayData + row) = data;

	switch (row) {
	case 0:
		code = DIGIT_0;
		break;
	case 1:
		code = DIGIT_1;
		break;
	case 2:
		code = DIGIT_2;
		break;
	case 3:
		code = DIGIT_3;
		break;
	case 4:
		code = DIGIT_4;
		break;
	case 5:
		code = DIGIT_5;
		break;
	case 6:
		code = DIGIT_6;
		break;
	case 7:
		code = DIGIT_7;
		break;
	}

	spiTransmit(code, data);

}

void write_a_text_on_screen(char *text, uint8_t sizeoftext) {

	if (sizeoftext > 21) {
		// do nothing
	} else {
		long long pixels[5] = { 0 };

		for (int i = 0; i < sizeoftext; i++) {
			uint8_t location_in_alph = text[i] - 65; // ASCI Value of A is 65
			for (int col = 0; col < 5; col++) {
				pixels[col] = pixels[col]
						| (((long long) Alphabet[location_in_alph][col])
								<< (64 - ((i + 1) * 3)));

			}
		}

		int first_one = 63;
		int last_one = 56;
		int turn_around_last = 0;

		for (;;) {

			uint8_t moving_text[5] = { 0 };

			if(turn_around_last){
				int rest_of_bits = last_one - 56;
				for (int i = 0; i < rest_of_bits; i++) {
					for (int j = 0; j < 5; j++) {
						int shift = first_one-i;
						moving_text[j] = moving_text[j] | (((pixels[j] >> shift) & 1) << (7-i));
					}

				}

				for (int i = rest_of_bits; i < 8; i++) {
					for (int j = 0; j < 5; j++) {
						int shift = 63 - (i - rest_of_bits);
						moving_text[j] = moving_text[j] | (((pixels[j] >> shift) & 1) << (7-i));
					}

				}

				if(rest_of_bits <= 1){
					turn_around_last = 0;
				}
			}else{
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 5; j++) {
						int shift = first_one-i;
						moving_text[j] = moving_text[j] | (((pixels[j] >> shift) & 1) << (7-i));


					}

				}

			}



			first_one = first_one - 1;
			last_one = last_one - 1;
			if (last_one <= 63 - (sizeoftext * 3) ){
				last_one = 63;
				turn_around_last = 1;

			}

			if (first_one <= 63 - (sizeoftext * 3) ){
				first_one = 63;
					}

			for (int waitin =0 ; waitin < 1000000; waitin++);
			for (int j = 0; j < 5; j++){
				spiTransmit(j + 1, moving_text[j]);

			}


		}

	}

}

void spiTransmit(uint8_t code, uint8_t data) {

	uint16_t transmittedData = (code << 8) | data;

	//make SS low
	GPIOB->ODR &= ~(1 << 12);

	// write the data in SPI_DR
	SPI2->DR = transmittedData;

	//wait until TXE_flag is 1 and then write the next frame
	while (!((SPI2->SR >> 1) & 1))
		;
	while (((SPI2->SR >> 7) & 1)) {
	};
	//make SS high
	GPIOB->ODR |= (1 << 12);
}

void spiDisable() {
	SPI2->CR1 &= ~(1 << 6);
}

