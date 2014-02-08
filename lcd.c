#include "tools.h"
#include "lcd.h"

#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t) 0x0001)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t) 0x0000)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t) 0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t) 0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t) 0x0200)

static FMC_SDRAMInitTypeDef FMC_SDRAMInitStruct;
static FMC_SDRAMTimingInitTypeDef FMC_SDRAMTimingInitStruct; 
static FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStruct;

static LTDC_InitTypeDef LTDC_InitStruct;
static LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

static uint16_t* LCD_front_buffer;
uint16_t* LCD_buffer;

static void enable_vram_pins(void) {
  set_gpio_af(PD, 0, GPIO_AF_FMC);
  set_gpio_af(PD, 1, GPIO_AF_FMC);
  set_gpio_af(PD, 8, GPIO_AF_FMC);
  set_gpio_af(PD, 9, GPIO_AF_FMC);
  set_gpio_af(PD, 10, GPIO_AF_FMC);
  set_gpio_af(PD, 14, GPIO_AF_FMC);
  set_gpio_af(PD, 15, GPIO_AF_FMC);

  set_gpio_af(PE, 0, GPIO_AF_FMC);
  set_gpio_af(PE, 1, GPIO_AF_FMC);
  set_gpio_af(PE, 2, GPIO_AF_FMC);
  set_gpio_af(PE, 3, GPIO_AF_FMC);
  set_gpio_af(PE, 4, GPIO_AF_FMC);
  set_gpio_af(PE, 5, GPIO_AF_FMC);
  set_gpio_af(PE, 6, GPIO_AF_FMC);
  set_gpio_af(PE, 7, GPIO_AF_FMC);
  set_gpio_af(PE, 8, GPIO_AF_FMC);
  set_gpio_af(PE, 9, GPIO_AF_FMC);
  set_gpio_af(PE, 10, GPIO_AF_FMC);
  set_gpio_af(PE, 11, GPIO_AF_FMC);
  set_gpio_af(PE, 12, GPIO_AF_FMC);
  set_gpio_af(PE, 13, GPIO_AF_FMC);
  set_gpio_af(PE, 14, GPIO_AF_FMC);
  set_gpio_af(PE, 15, GPIO_AF_FMC);

  set_gpio_af(PF, 0, GPIO_AF_FMC);
  set_gpio_af(PF, 1, GPIO_AF_FMC);
  set_gpio_af(PF, 2, GPIO_AF_FMC);
  set_gpio_af(PF, 3, GPIO_AF_FMC);
  set_gpio_af(PF, 4, GPIO_AF_FMC);
  set_gpio_af(PF, 5, GPIO_AF_FMC);
  set_gpio_af(PF, 6, GPIO_AF_FMC);
  set_gpio_af(PF, 7, GPIO_AF_FMC);
  set_gpio_af(PF, 8, GPIO_AF_FMC);
  set_gpio_af(PF, 9, GPIO_AF_FMC);
  set_gpio_af(PF, 10, GPIO_AF_FMC);
  set_gpio_af(PF, 11, GPIO_AF_FMC);
  set_gpio_af(PF, 12, GPIO_AF_FMC);
  set_gpio_af(PF, 13, GPIO_AF_FMC);
  set_gpio_af(PF, 14, GPIO_AF_FMC);
  set_gpio_af(PF, 15, GPIO_AF_FMC);

  set_gpio_af(PG, 0, GPIO_AF_FMC);
  set_gpio_af(PG, 1, GPIO_AF_FMC);
  set_gpio_af(PG, 8, GPIO_AF_FMC);
  set_gpio_af(PG, 15, GPIO_AF_FMC);

  set_gpio_af(PB, 5, GPIO_AF_FMC);
  set_gpio_af(PB, 6, GPIO_AF_FMC);
  set_gpio_af(PC, 0, GPIO_AF_FMC);
}

static void enable_vram(void) {
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

  FMC_SDRAMTimingInitStruct.FMC_LoadToActiveDelay = 2;
  FMC_SDRAMTimingInitStruct.FMC_ExitSelfRefreshDelay = 7;
  FMC_SDRAMTimingInitStruct.FMC_SelfRefreshTime = 4;
  FMC_SDRAMTimingInitStruct.FMC_RowCycleDelay = 7;   
  FMC_SDRAMTimingInitStruct.FMC_WriteRecoveryTime = 2;
  FMC_SDRAMTimingInitStruct.FMC_RPDelay = 2;
  FMC_SDRAMTimingInitStruct.FMC_RCDDelay = 2;

  FMC_SDRAMInitStruct.FMC_Bank = FMC_Bank2_SDRAM;
  FMC_SDRAMInitStruct.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
  FMC_SDRAMInitStruct.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
  FMC_SDRAMInitStruct.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
  FMC_SDRAMInitStruct.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  FMC_SDRAMInitStruct.FMC_CASLatency = FMC_CAS_Latency_3;
  FMC_SDRAMInitStruct.FMC_WriteProtection = FMC_Write_Protection_Disable;
  FMC_SDRAMInitStruct.FMC_SDClockPeriod = FMC_SDClock_Period_2;
  FMC_SDRAMInitStruct.FMC_ReadBurst = FMC_Read_Burst_Disable;
  FMC_SDRAMInitStruct.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  FMC_SDRAMInitStruct.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStruct;
  FMC_SDRAMInit(&FMC_SDRAMInitStruct);

  FMC_SDRAMCommandStruct.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
  FMC_SDRAMCommandStruct.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStruct.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStruct.FMC_ModeRegisterDefinition = 0;
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStruct);
  
  delay_ms(10);
    
  FMC_SDRAMCommandStruct.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStruct.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStruct.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStruct.FMC_ModeRegisterDefinition = 0;
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStruct);
  
  FMC_SDRAMCommandStruct.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
  FMC_SDRAMCommandStruct.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStruct.FMC_AutoRefreshNumber = 4;
  FMC_SDRAMCommandStruct.FMC_ModeRegisterDefinition = 0;
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStruct);
  
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStruct);
  
  FMC_SDRAMCommandStruct.FMC_CommandMode = FMC_Command_Mode_LoadMode;
  FMC_SDRAMCommandStruct.FMC_CommandTarget = FMC_Command_Target_bank2;
  FMC_SDRAMCommandStruct.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStruct.FMC_ModeRegisterDefinition = (uint32_t)
    SDRAM_MODEREG_BURST_LENGTH_2 |
    SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
    SDRAM_MODEREG_CAS_LATENCY_3 |
    SDRAM_MODEREG_OPERATING_MODE_STANDARD |
    SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStruct);

  FMC_SetRefreshCount(1386);
  while (FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET) {
  }
}

static void enable_lcd_pins(void) {
  set_gpio_af(PF, 10, GPIO_AF_LTDC);
  set_gpio_af(PG, 7, GPIO_AF_LTDC);

  set_gpio_af(PB, 0, 9);
  set_gpio_af(PA, 11, GPIO_AF_LTDC);
  set_gpio_af(PA, 12, GPIO_AF_LTDC);
  set_gpio_af(PB, 1, 9);
  set_gpio_af(PG, 6, GPIO_AF_LTDC);

  set_gpio_af(PA, 6, GPIO_AF_LTDC);
  set_gpio_af(PG, 10, 9);
  set_gpio_af(PB, 10, GPIO_AF_LTDC);
  set_gpio_af(PB, 11, GPIO_AF_LTDC);
  set_gpio_af(PC, 7, GPIO_AF_LTDC);
  set_gpio_af(PD, 3, GPIO_AF_LTDC);

  set_gpio_af(PG, 11, GPIO_AF_LTDC);
  set_gpio_af(PG, 12, 9);
  set_gpio_af(PA, 3, GPIO_AF_LTDC);
  set_gpio_af(PB, 8, GPIO_AF_LTDC);
  set_gpio_af(PB, 9, GPIO_AF_LTDC);
}

static void enable_lcd_clock(void) {
  RCC_PLLSAIConfig(216, 7, 7);
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
  RCC_PLLSAICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET) {
  }
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
}

static void enable_lcd(void) {
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  LTDC_InitStruct.LTDC_HorizontalSync = HSYNC - 1;
  LTDC_InitStruct.LTDC_VerticalSync = VSYNC - 1;
  LTDC_InitStruct.LTDC_AccumulatedHBP = HSYNC + HBP - 1;
  LTDC_InitStruct.LTDC_AccumulatedVBP = VSYNC + VBP - 1;
  LTDC_InitStruct.LTDC_AccumulatedActiveW = HSYNC + HBP + LCD_WIDTH - 1;
  LTDC_InitStruct.LTDC_AccumulatedActiveH = VSYNC + VBP + LCD_HEIGHT - 1;
  LTDC_InitStruct.LTDC_TotalWidth = HSYNC + HBP + LCD_WIDTH + HFP - 1;
  LTDC_InitStruct.LTDC_TotalHeigh = VSYNC + VBP + LCD_HEIGHT + VFP - 1;
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;
  LTDC_Init(&LTDC_InitStruct);

  LTDC_Layer_InitStruct.LTDC_HorizontalStart = HSYNC + HBP;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSYNC + HBP + LCD_WIDTH - 1;
  LTDC_Layer_InitStruct.LTDC_VerticalStart = VSYNC + VBP;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = VSYNC + VBP + LCD_HEIGHT - 1;
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t) LCD_front_buffer;
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = LCD_WIDTH * 2 + 3;
  LTDC_Layer_InitStruct.LTDC_CFBPitch = LCD_WIDTH * 2;
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_HEIGHT;
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
  LTDC_ReloadConfig(LTDC_IMReload);
  LTDC_Cmd(ENABLE);
}

void begin_draw(void) {
  while (LTDC->SRCR & LTDC_VBReload) {
  }
}

void end_draw(void) {
  uint16_t* t;
  t = LCD_front_buffer;
  LCD_front_buffer = LCD_buffer;
  LCD_buffer = t;
  LTDC_LayerAddress(LTDC_Layer1, (uint32_t) LCD_front_buffer);
  LTDC_ReloadConfig(LTDC_VBReload);
}

static void enable_dma2d(void) {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
}

void configure_lcd(void) {
  LCD_front_buffer = VRAM;
  LCD_buffer = VRAM + LCD_WIDTH * LCD_HEIGHT * 2;
  enable_vram_pins();
  enable_vram();
  enable_lcd_pins();
  enable_lcd_clock();
  enable_lcd();
  enable_dma2d();
}
