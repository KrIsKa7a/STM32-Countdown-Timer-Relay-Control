#include "stm32f7xx.h"
#include "gpio.h"
#include "systick.h"

#define RCC_AHB1ENR_GPIOA_EN		(1U<<0)
#define RCC_AHB1ENR_GPIOB_EN		(1U<<1)
#define RCC_AHB1ENR_GPIOC_EN		(1U<<2)

#define GPIOB_ODR_PIN7				(1U<<7)
#define GPIOB_ODR_PIN14				(1U<<14)
#define GPIOC_DP					(1U<<7)
#define GPIOA_RELAY					(1U<<4)

static void GPIOA_init_D1_to_D4(void);
static void GPIOC_init_A_to_F(void);
static void get_digits_from_number(int number, uint8_t* numDigits);
static void GPIOC_display_digit(uint8_t digit);
static void GPIOA_Dx_On(uint8_t dx);
static void GPIOA_Dx_Off(uint8_t dx);

static uint16_t digitMasks[] =
{
		0b0111111, //0
		0b0000110, //1
		0b1011011, //2
		0b1001111, //3
		0b1100110, //4
		0b1101101, //5
		0b1111101, //6
		0b0000111, //7
		0b1111111, //8
		0b1101111, //9
};

void GPIOB_pin7_pin14_output_init(void) {
	// Enable clock access for GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOB_EN;

	// Set mode of PIN14 to Output Push-Pull
	GPIOB->MODER |= (1U<<28);
	GPIOB->MODER &= ~(1U<<29);

	// Set mode of PIN7 to Output Push-Pull
	GPIOB->MODER |= (1U<<14);
	GPIOB->MODER &= ~(1U<<15);
}

void GPIOB_pin7_set(void) {
	GPIOB->ODR |= GPIOB_ODR_PIN7;
}

void GPIOB_pin7_reset(void) {
	GPIOB->ODR &= ~GPIOB_ODR_PIN7;
}

void GPIOB_pin14_set(void) {
	GPIOB->ODR |= GPIOB_ODR_PIN14;
}

void GPIOB_pin14_reset(void) {
	GPIOB->ODR &= ~GPIOB_ODR_PIN14;
}

void GPIOA_GPIOC_7segment_init(void) {
	// Enable clock access for GPIOA peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOA_EN;
	GPIOA_init_D1_to_D4();

	// Enable clock access for GPIOC peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOC_EN;
	GPIOC_init_A_to_F();

	// Turn off all digits
	GPIOA_Dx_Off(0); //D1
	GPIOA_Dx_Off(1); //D2
	GPIOA_Dx_Off(2); //D3
	GPIOA_Dx_Off(3); //D4
}

void GPIOA_GPIOC_7segment_number_display(int number) {
	uint8_t numDigits[4] = {0};
	get_digits_from_number(number, numDigits);

	for (int dx = 0; dx < 4; dx++) {
		uint8_t currentDigit = numDigits[3 - dx];

		GPIOC_display_digit(currentDigit);
		if (dx == 1) {
			GPIOC->ODR |= GPIOC_DP;
		}

		GPIOA_Dx_On(dx);
		systick_delay_ms(3);
		GPIOA_Dx_Off(dx);
	}
}

void GPIOA_GPIOC_7segment_EOC() {
	GPIOC_display_digit(0);

	for (int dx = 0; dx < 4; dx++) {
		GPIOA_Dx_On(dx);
	}

	systick_delay_ms(700);

	for (int dx = 0; dx < 4; dx++) {
		GPIOA_Dx_Off(dx);
	}

	systick_delay_ms(700);
}

void GPIOA_pin4_relay_ctrl_init(void) {
	// A4 -> Relay control
	GPIOA->MODER |= (1U<<8);
	GPIOA->MODER &= ~(1U<<9);
}

void GPIOA_relay_activate(void) {
	GPIOA->ODR |= GPIOA_RELAY;
	GPIOB_pin14_set();
}

void GPIOA_relay_deactivate(void) {
	GPIOA->ODR &= ~GPIOA_RELAY;
	GPIOB_pin14_reset();
}

static void get_digits_from_number(int number, uint8_t* numDigits) {
	while (number > 0) {
		*numDigits = number % 10;
		number /= 10;
		numDigits++;
	}
}

static void GPIOC_display_digit(uint8_t digit) {
	GPIOC->ODR = 0;

	uint16_t digitMask = digitMasks[digit];
	GPIOC->ODR |= digitMask;
}

static void GPIOA_Dx_On(uint8_t dx) {
	GPIOA->ODR &= ~(1U<<dx);
}

static void GPIOA_Dx_Off(uint8_t dx) {
	GPIOA->ODR |= (1U<<dx);
}

static void GPIOA_init_D1_to_D4(void) {
	// Connections:
	// A0 -> D1
	// A1 -> D2
	// A2 -> D3
	// A3 -> D4

	// D1 -> Output
	GPIOA->MODER |= (1U<<0);
	GPIOA->MODER &= ~(1U<<1);

	// D2 -> Output
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER &= ~(1U<<3);

	// D3 -> Output
	GPIOA->MODER |= (1U<<4);
	GPIOA->MODER &= ~(1U<<5);

	// D4 -> Output
	GPIOA->MODER |= (1U<<6);
	GPIOA->MODER &= ~(1U<<7);
}

static void GPIOC_init_A_to_F(void) {
	// Connections:
	// C0 -> A
	// C1 -> B
	// C2 -> C
	// C3 -> D
	// C4 -> E
	// C5 -> F
	// C6 -> G
	// C7 -> DP
	// A -> Output
	GPIOC->MODER |= (1U<<0);
	GPIOC->MODER &= ~(1U<<1);

	// B -> Output
	GPIOC->MODER |= (1U<<2);
	GPIOC->MODER &= ~(1U<<3);

	// C -> Output
	GPIOC->MODER |= (1U<<4);
	GPIOC->MODER &= ~(1U<<5);

	// D -> Output
	GPIOC->MODER |= (1U<<6);
	GPIOC->MODER &= ~(1U<<7);

	// E -> Output
	GPIOC->MODER |= (1U<<8);
	GPIOC->MODER &= ~(1U<<9);

	// F -> Output
	GPIOC->MODER |= (1U<<10);
	GPIOC->MODER &= ~(1U<<11);

	// G -> Output
	GPIOC->MODER |= (1U<<12);
	GPIOC->MODER &= ~(1U<<13);

	// DP -> Output
	GPIOC->MODER |= (1U<<14);
	GPIOC->MODER &= ~(1U<<15);
}
