#include "stm32f7xx.h"
#include "tim.h"

#define APB1_TIM2_EN	(1U<<0)
#define APB1_TIM3_EN	(1U<<1)
#define CR1_CEN			(1U<<0)
#define SR_UIF			(1U<<0)
#define DIER_UIE		(1U<<0)
#define TIM2_PSC		(1600 - 1)
#define TIM3_PSC		(16000 - 1)
#define TIM2_ARR		(10000 - 1)

void tim2_1s_count_interrupt_init(void) {
	// Enable clock for TIM2 peripheral
	RCC->APB1ENR |= APB1_TIM2_EN;

	// Set clock prescaler
	TIM2->PSC = TIM2_PSC; // 16 000 000 / 1600 = 10 000

	// Set counter Auto-Reload Value
	TIM2->ARR = TIM2_ARR;

	// Enable the counter in TIM2 CR1
	TIM2->CR1 |= CR1_CEN;

	// Enable interrupts for TIM2
	TIM2->DIER |= DIER_UIE;

	// Enable interrupt in the NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
}


void tim2_1s_count_stop(void) {
	TIM2->CR1 &= ~CR1_CEN;
}
