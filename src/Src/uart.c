#include "stm32f7xx.h"
#include "stdio.h"
#include "uart.h"
#include "gpio.h"

#define RCC_AHB1ENR_GPIOD_EN    (1U<<3)
#define RCC_APB1ENR_USART3_EN   (1U<<18)

#define CR1_UE			(1U<<0)
#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)
#define CR1_RXNEIE		(1U<<5)
#define CR2_ABREN		(1U<<20)

#define ISR_TC			(1U<<6)
#define ISR_TXE			(1U<<7)
#define ISR_RXNE		(1U<<5)

#define APB1_CLK    	16000000
#define USART_BAUDRATE  115200

static uint16_t compute_baudrate(uint32_t periphClk, uint32_t baudRate);
static void usart_set_baudrate(USART_TypeDef* USARTx, uint32_t periphClk, uint32_t baudRate);

void usart3_rx_tx_init(void) {
	// Enable clock access for GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOD_EN;

	// Set PD8 as alternate function mode
	GPIOD->MODER &= ~(1U<<16);
	GPIOD->MODER |= (1U<<17);

	// Set the type of the alternate function
	GPIOD->AFR[1] |= (1U<<0);
	GPIOD->AFR[1] |= (1U<<1);
	GPIOD->AFR[1] |= (1U<<2);
	GPIOD->AFR[1] &= ~(1U<<3);

	// Set PD9 as alternate function mode
	GPIOD->MODER &= ~(1U<<18);
	GPIOD->MODER |= (1U<<19);

	// Set the type of the alternate function
	GPIOD->AFR[1] |= (1U<<4);
	GPIOD->AFR[1] |= (1U<<5);
	GPIOD->AFR[1] |= (1U<<6);
	GPIOD->AFR[1] &= ~(1U<<7);

	// Enable clock access for USART3
	RCC->APB1ENR |= RCC_APB1ENR_USART3_EN;

	// Configure Baudrate
	usart_set_baudrate(USART3, APB1_CLK, USART_BAUDRATE);

	// Configure transfer direction
	USART3->CR1 = (CR1_TE | CR1_RE);
	USART3->CR1 |= CR1_UE;

	// Auto baud rate detenction if needed
	// USART3->CR2 |= CR2_ABREN;
	// usart3_read();
}

uint8_t usart3_read(void) {
	while(!(USART3->ISR & ISR_RXNE)) {

	}

	return USART3->RDR;
}

uint8_t usart3_read_countdown_start(int number) {
	while(!(USART3->ISR & ISR_RXNE)) {
		GPIOA_GPIOC_7segment_number_display(number);
	}

	return USART3->RDR;
}

void usart3_write_ch(int ch) {
	// Waiting for the data to be fully transmitted to the shift register
	while (!(USART3->ISR & ISR_TXE)) {

	}

	// Conversion to 8-bit using 0xFF
	USART3->TDR = (ch & 0xFF);
}

void usart3_write(char* str, int len) {
	for (int i = 0; i < len; i++) {
		char currCh = *str++;

		usart3_write_ch(currCh);
	}
}

void usart3_writeLine(char* str, int len) {
	for (int i = 0; i < len; i++) {
		char currCh = *str++;

		usart3_write_ch(currCh);
	}

	usart3_write_ch('\r');
	usart3_write_ch('\n');
}

static void usart_set_baudrate(USART_TypeDef* USARTx, uint32_t periphClk, uint32_t baudRate) {
	USARTx->BRR = compute_baudrate(periphClk, baudRate);
}

static uint16_t compute_baudrate(uint32_t periphClk, uint32_t baudRate) {
	return periphClk / baudRate;
}
