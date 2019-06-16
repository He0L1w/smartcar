#include "system.h"

void ExInt_Init(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);          // IO口时钟使能
  
  IOMUXC_SetPinMux(IOMUXC_GPIO_B1_08_GPIO2_IO24,0U);    //A12
  IOMUXC_SetPinMux(IOMUXC_GPIO_B1_09_GPIO2_IO25,0U);    //A13
  //IOMUXC_SetPinMux(DISTANCE_INT_PINMUX,0U);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_08_GPIO2_IO24,0xF080);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_09_GPIO2_IO25,0xF080);
  //IOMUXC_SetPinConfig(DISTANCE_INT_PINMUX,0xF080);

  /*GPIO 外部中断配置开始*/  
  gpio_pin_config_t GPIO_Input_Config =                     // GPIO初始化结构体
  {
    kGPIO_DigitalInput,                                     // GPIO为输入方向
    0,                                                      // 高电平
    kGPIO_IntRisingEdge,                                   // 下降沿触发中断
  };
 
  /* 终点磁铁触发中断 */
  GPIO_PinInit(GPIO2, 24, &GPIO_Input_Config);
  GPIO_PortEnableInterrupts(GPIO2,1<<24);
  
  /* 远程遥控触发中断 */
  GPIO_PinInit(GPIO2, 25, &GPIO_Input_Config);
  GPIO_PortEnableInterrupts(GPIO2,1<<25);
  
  //优先级配置 抢占优先级1  子优先级2   越小优先级越高  抢占优先级可打断别的中断
  NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1,2));
  EnableIRQ(GPIO2_Combined_16_31_IRQn);			          //使能GPIO2_16~31 IO的中断 
  
}

void GPIO2_Combined_16_31_IRQHandler(void)
{ 
  delayms(10);      //消抖
  
  /* clear the interrupt status */
  if(GPIO_GetPinsInterruptFlags(GPIO2)&(1<<24))    //磁铁触发中断
  {
    speedvalue = 0;
    GPIO_PortClearInterruptFlags(GPIO2, 1U << 24);   //清除标志位
  }
  
  if(GPIO_GetPinsInterruptFlags(GPIO2)&(1<<25))    //遥控触发中断
  {
    left_motor(0);      //先让车停下
    right_motor(0);
    
    UI_debugsetting();//调用交互界面，重新设置参数
    GPIO_PortClearInterruptFlags(GPIO2, 1U << 25);   //清除标志位
  }  
  
/* Change state of switch. */
  __DSB();				//数据同步屏蔽指令
}


