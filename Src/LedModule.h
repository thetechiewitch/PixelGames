

#ifndef LED_MODULE_H
#define LED_MODULE_H


#define CODE_SHUTDOWN 		0b1100
#define CODE_INTENSITY		0b1010
#define CODE_SCANLIMIT  	0b1011
#define CODE_DISPLAYTEST	0b1111
#define CODE_DECODEMODE		0b1001
#define DIGIT_0				0b0001
#define DIGIT_1				0b0010
#define DIGIT_2				0b0011
#define DIGIT_3				0b0100
#define DIGIT_4				0b0101
#define DIGIT_5				0b0110
#define DIGIT_6				0b0111
#define DIGIT_7				0b1000

#define INTENSITY_LEVLMN    0b0000
#define INTENSITY_LEVEL1    0b0001
#define INTENSITY_LEVEL2    0b0010
#define INTENSITY_LEVEL3    0b0011
#define INTENSITY_LEVEL4    0b0100
#define INTENSITY_LEVEL5    0b0101
#define INTENSITY_LEVEL6    0b0110
#define INTENSITY_LEVEL7    0b0111
#define INTENSITY_LEVEL8    0b1000
#define INTENSITY_LEVEL9    0b1001
#define INTENSITY_LEVEL10   0b1010
#define INTENSITY_LEVEL11   0b1011
#define INTENSITY_LEVEL12   0b1100
#define INTENSITY_LEVEL13   0b1101
#define INTENSITY_LEVEL14   0b1110
#define INTENSITY_LEVEL15   0b1111





void CLOCK_init();
void SPI_init();
void shutdown(bool onOff);
void spiDisable();
void spiTransmit(uint8_t code, uint8_t data);
void clearDisplay(uint8_t *savedDisplayData);
void pixels_on(uint8_t row, uint8_t column, uint8_t *savedDisplayData); // Select number between 1 and 8, or else you will have a buggy result on display
void pixels_off(uint8_t row, uint8_t column, uint8_t *savedDisplayData);
void all_pixels_on(uint8_t *savedDisplayData);
void write_a_text_on_screen(char *text, uint8_t sizeoftext);



#endif
