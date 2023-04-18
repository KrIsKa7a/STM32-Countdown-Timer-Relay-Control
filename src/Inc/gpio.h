#ifndef GPIO_H_
#define GPIO_H_

#include "stdint.h"

void GPIOB_pin7_pin14_output_init(void);
void GPIOB_pin7_set(void);
void GPIOB_pin7_reset(void);
void GPIOB_pin14_set(void);
void GPIOB_pin14_reset(void);

void GPIOA_GPIOC_7segment_init(void);
void GPIOA_GPIOC_7segment_number_display(int number);
void GPIOA_GPIOC_7segment_EOC();

void GPIOA_pin4_relay_ctrl_init(void);
void GPIOA_relay_activate(void);
void GPIOA_relay_deactivate(void);

#endif /* GPIO_H_ */
