#include "stm32f7xx.h"
#include "gpio.h"
#include "uart.h"
#include "tim.h"
#include "systick.h"
#include "stdint.h"
#include "string.h"

#define TIM2_SR_UIF		(1U<<0)

volatile uint16_t secondsPassed = 0;
volatile uint16_t finalCountdown = 0;

void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM2_SR_UIF;

	secondsPassed++;
}

void read_start_countdown_value(void);

int main(void)
{
	GPIOB_pin7_pin14_output_init();
	GPIOA_GPIOC_7segment_init();
	GPIOA_pin4_relay_ctrl_init();
	usart3_rx_tx_init();

	GPIOB_pin7_set();

	read_start_countdown_value();

	tim2_1s_count_interrupt_init();
	GPIOA_relay_activate();
	while (1) {
		uint16_t secondsToDisplay = finalCountdown - secondsPassed;
		uint8_t minutes = secondsToDisplay / 60;
		uint8_t seconds = secondsToDisplay % 60;
		uint16_t numberToDisplay = ((minutes / 10) * 1000) + ((minutes % 10) * 100) +
				((seconds / 10) * 10) + (seconds % 10);

		GPIOA_GPIOC_7segment_number_display(numberToDisplay);

		if (secondsPassed == finalCountdown) {
			tim2_1s_count_stop();
			GPIOA_GPIOC_7segment_EOC();
			GPIOA_relay_deactivate();
		}
	}
}

void read_start_countdown_value(void) {
	char m1, m2, s1, s2;
	while (1) {
		char* startMenuMsg = "Enter start for countdown in format MM:SS: ";
		usart3_write(startMenuMsg, strlen(startMenuMsg));

		m1 = usart3_read();
		usart3_write_ch(m1);
		m2 = usart3_read();
		usart3_write_ch(m2);
		usart3_read(); // dummy :
		usart3_write_ch(':');
		s1 = usart3_read();
		usart3_write_ch(s1);
		s2 = usart3_read();
		usart3_write_ch(s2);

		if (((uint32_t)s1 - 48) <= 5) {
			break;
		}
	}

	uint8_t minutes = ((uint8_t)m1 - 48) * 10 + ((uint8_t)m2 - 48);
	uint8_t seconds = ((uint8_t)s1 - 48) * 10 + ((uint8_t)s2 - 48);

	finalCountdown = minutes * 60 + seconds;

	uint16_t displayTime = ((minutes / 10) * 1000) + ((minutes % 10) * 100) +
			((seconds / 10) * 10) + (seconds % 10);
	GPIOA_GPIOC_7segment_number_display(displayTime);

	char* countdownStartMsg = "\r\nPress any key to start countdown...";
	usart3_writeLine(countdownStartMsg, strlen(countdownStartMsg));

	usart3_read_countdown_start(displayTime);
}
