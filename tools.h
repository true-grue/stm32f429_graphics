#ifndef TOOLS_H
#define TOOLS_H

#include "stm32f4xx_conf.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define LIMIT(x, low, high) MIN(MAX(x, low), high)

#define PA GPIOA
#define PB GPIOB
#define PC GPIOC
#define PD GPIOD
#define PE GPIOE
#define PF GPIOF
#define PG GPIOG

#define PIN(x) (1 << (x))
#define PIN_SOURCE(x) (x)

#define DEMCR ((volatile uint32_t*) 0xe000edfc)
#define DWT_CTRL ((volatile uint32_t*) 0xe0001000)
#define DWT_CYCCNT ((volatile uint32_t*) 0xe0001004)

void delay_us(uint32_t interval);
void delay_ms(uint32_t interval);
uint32_t get_random(void);
void set_gpio_nopull(GPIO_TypeDef* GPIOx,
  uint16_t pin,
  GPIOMode_TypeDef mode,
  GPIOOType_TypeDef type);
void set_gpio_out(GPIO_TypeDef* GPIOx, uint16_t pin);
void set_gpio_af(GPIO_TypeDef* GPIOx, uint16_t pin, uint8_t GPIO_AF);
void set_timer_data(TIM_TypeDef* TIMx, uint16_t prescaler, uint32_t period);
void set_timer_rate(TIM_TypeDef* TIMx, uint32_t hz);
void set_timer(TIM_TypeDef* TIMx, uint32_t hz);
void set_pwm(TIM_TypeDef* TIMx, int channel, uint32_t pulse);
void set_nvic(uint8_t channel, uint8_t priority, uint8_t subpriority);
void configure_tools(void);

#endif
