#include "tools.h"

static GPIO_InitTypeDef GPIO_InitStruct;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
static TIM_OCInitTypeDef TIM_OCInitStruct;
static NVIC_InitTypeDef NVIC_InitStruct;

static uint32_t Cycles_us;

static uint32_t Seed = 2463534242u;

static void enable_dwt_counter(void) {
  *DEMCR |= 0x01000000;
  *DWT_CYCCNT = 0;
  *DWT_CTRL |= 1;
}

void delay_us(uint32_t interval) {
  uint32_t old_cyccnt;

  interval *= Cycles_us;
  old_cyccnt = *DWT_CYCCNT;
  while ((*DWT_CYCCNT - old_cyccnt) < interval) {
  }
}

void delay_ms(uint32_t interval) {
  while (interval) {
    delay_us(1000);
    interval -= 1;
  }
}

uint32_t get_random(void) {
  Seed ^= Seed << 13;
  Seed ^= Seed >> 17;
  Seed ^= Seed << 15;
  return Seed;
}

void set_gpio_nopull(GPIO_TypeDef* GPIOx,
  uint16_t pin,
  GPIOMode_TypeDef mode,
  GPIOOType_TypeDef type) {
  GPIO_InitStruct.GPIO_Pin = PIN(pin);
  GPIO_InitStruct.GPIO_Mode = mode;
  GPIO_InitStruct.GPIO_OType = type;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void set_gpio_out(GPIO_TypeDef* GPIOx, uint16_t pin) {
  set_gpio_nopull(GPIOx, pin, GPIO_Mode_OUT, GPIO_OType_PP);
}

void set_gpio_af(GPIO_TypeDef* GPIOx, uint16_t pin, uint8_t GPIO_AF) {
  set_gpio_nopull(GPIOx, pin, GPIO_Mode_AF, GPIO_OType_PP);
  GPIO_PinAFConfig(GPIOx, PIN_SOURCE(pin), GPIO_AF);
}

void set_timer_data(TIM_TypeDef* TIMx, uint16_t prescaler, uint32_t period) {
  TIM_TimeBaseInitStruct.TIM_Period = period;
  TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct);
  TIM_ARRPreloadConfig(TIMx, ENABLE);
}

void set_timer_rate(TIM_TypeDef* TIMx, uint32_t hz) {
  uint32_t period;
  uint32_t prescaler = 1;

  if (TIMx == TIM1 || TIMx == TIM8 || TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11) {
    period = SystemCoreClock / hz;
  } else {
    period = (SystemCoreClock / 2) / hz;
  }
  while (period > 65536) {
    period >>= 1;
    prescaler <<= 1;
  }
  TIM_SetAutoreload(TIMx, MAX(period - 1, 1));
  TIM_PrescalerConfig(TIMx, prescaler - 1, TIM_PSCReloadMode_Update);
}

void set_timer(TIM_TypeDef* TIMx, uint32_t hz) {
  set_timer_data(TIMx, 0, 1);
  set_timer_rate(TIMx, hz);
}

static void set_pwm_channel(TIM_TypeDef* TIMx, int channel) {
  if (channel == 1) {
    TIM_OC1Init(TIMx, &TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
  } else if (channel == 2) {
    TIM_OC2Init(TIMx, &TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
  } else if (channel == 3) {
    TIM_OC3Init(TIMx, &TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
  } else if (channel == 4) {
    TIM_OC4Init(TIMx, &TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
  }
}

void set_pwm(TIM_TypeDef* TIMx, int channel, uint32_t pulse) {
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct.TIM_Pulse = pulse;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  set_pwm_channel(TIMx, channel);
  if (TIMx == TIM1 || TIMx == TIM8) {
    TIM_CtrlPWMOutputs(TIMx, ENABLE);
  }
}

void set_nvic(uint8_t channel, uint8_t priority, uint8_t subpriority) {
  NVIC_InitStruct.NVIC_IRQChannel = channel;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = priority;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = subpriority;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

void configure_tools(void) {
  enable_dwt_counter();
  Cycles_us = SystemCoreClock / 1000000;
  RCC_AHB1PeriphClockCmd(
    RCC_AHB1Periph_GPIOA |
    RCC_AHB1Periph_GPIOB |
    RCC_AHB1Periph_GPIOC |
    RCC_AHB1Periph_GPIOD |
    RCC_AHB1Periph_GPIOE |
    RCC_AHB1Periph_GPIOF |
    RCC_AHB1Periph_GPIOG,
    ENABLE);
}
