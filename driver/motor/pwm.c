/**
 *  pwm.c
 *  ----------------
 *  pwm模块初始化
 *  
 *  MT1接口  3V A9 A8 M5 L5
 *  PWM2, kPWM_Module_0, kPWM_PwmA   A8     电机2接口
 *  PWM2, kPWM_Module_0, kPWM_PwmB   A9     电机2接口
 *  PWM1, kPWM_Module_3, kPWM_PwmA   L5     电机1接口
 *  PWM1, kPWM_Module_3, kPWM_PwmB   M5     电机1接口
 *  
 *  舵机接口M3
 *  PWM2, kPWM_Module_3, kPWM_PwmA   M3
 *  
 */

#include "system.h"

static void PWM_UpdateDuty(PWM_Type *base,
                           pwm_submodule_t subModule,
                           pwm_channels_t pwmSignal,
                           uint16_t duty)
{
  uint16_t pulseCnt = 0, pwmHighPulse = 0;
  int16_t modulo = 0;
  modulo = base->SM[subModule].VAL1;
  pulseCnt = modulo * 2;
  pwmHighPulse = (pulseCnt * duty) / DUTY_MAX; /* pulse width */
  /* Setup the PWM dutycycle */
  if (pwmSignal == kPWM_PwmA)
  {
    base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
    base->SM[subModule].VAL3 = (pwmHighPulse / 2);
  }
  else
  {
    base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
    base->SM[subModule].VAL5 = (pwmHighPulse / 2);
  }
}


/**
 *  设置电机占空比（速度）
 *  ----------------
 *  duty                占空比             -DUTY_MAX ~ DUTY_MAX
 *  ----------------
 *  duty最大值10000定义在DUTY_MAX
 *  
 *  left_motor(-2000)  //左电机反转  占空比  2000 / 10000 
 */
void left_motor(short duty)
{
  if(duty >= 0)  //  正转
  {
    PWM_UpdateDuty(PWM1, kPWM_Module_3, kPWM_PwmA, 0); 
    PWM_UpdateDuty(PWM1, kPWM_Module_3, kPWM_PwmB, duty); 
    PWM_SetPwmLdok(PWM1, 1u<<kPWM_Module_3, true);        //设置pwm的 load ok位
  }
  else
  {
    PWM_UpdateDuty(PWM1, kPWM_Module_3, kPWM_PwmA, -duty); 
    PWM_UpdateDuty(PWM1, kPWM_Module_3, kPWM_PwmB, 0); 
    PWM_SetPwmLdok(PWM1, 1u<<kPWM_Module_3, true);        //设置pwm的 load ok位                
  }
}
void right_motor(short duty)
{
  if(duty >= 0)  //  正转
  {
    PWM_UpdateDuty(PWM2, kPWM_Module_0, kPWM_PwmA, 0); 
    PWM_UpdateDuty(PWM2, kPWM_Module_0, kPWM_PwmB, duty); 
    PWM_SetPwmLdok(PWM2, 1u<<kPWM_Module_0, true);        //设置pwm的 load ok位
  }
  else
  {
    PWM_UpdateDuty(PWM2, kPWM_Module_0, kPWM_PwmA, -duty); 
    PWM_UpdateDuty(PWM2, kPWM_Module_0, kPWM_PwmB, 0); 
    PWM_SetPwmLdok(PWM2, 1u<<kPWM_Module_0, true);        //设置pwm的 load ok位
  }
}
/**
 *  设置舵机pwm(角度)
 *  ----------------
 *  duty   2600-2400
 *  ----------------
 *  <3000 车左转，>3000车右转
 *  
 */
void servo(uint16_t duty)  
{
    if(duty > 3400) duty = 3400;
    if(duty < 2600) duty = 2600;
    PWM_UpdateDuty(PWM2, kPWM_Module_3, kPWM_PwmA, duty);
    PWM_SetPwmLdok(PWM2, 1u<<kPWM_Module_3, true);  
}


/**
 *  PWM引脚配置
 */
static void pwm_pinconfig(void)
{
  CLOCK_EnableClock(kCLOCK_Iomuxc);          /* 打开io时钟 */
  
  IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_00_FLEXPWM1_PWMA03, 0U);   //L5
  IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_01_FLEXPWM1_PWMB03, 0U);   //M5
  IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_FLEXPWM2_PWMA00, 0U);      //A8
  IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_FLEXPWM2_PWMB00, 0U);      //A9
  IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03, 0U);   //M3
  IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03, 0U);   //M4
  
  IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_00_FLEXPWM1_PWMA03, 0x10B0u);  
  IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_01_FLEXPWM1_PWMB03, 0x10B0u); 
  IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_FLEXPWM2_PWMA00, 0x10B0u); 
  IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_FLEXPWM2_PWMB00, 0x10B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03, 0x10B0u); 
  IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03, 0x10B0u);
}

/**
 *  PWM模式配置
 *  ----------------
 *  PWMclock 200MHz的32分频 = 6,250,000  PWM->VAL1 16bit 6,250,000/VAL1  = 96Hz
 *  96hz以下的PWM信号，可以降低PWM的时钟频率 
 */
static void pwm_config(void)
{
  CLOCK_SetDiv(kCLOCK_AhbDiv, 0x0); /* Set AHB PODF to 0, divide by 1 */ //600MHz 
  CLOCK_SetDiv(kCLOCK_IpgDiv, 0x2); /* Set IPG PODF to 2, divede by 3 */ //600MHz 3分频 200MHz
  
  uint32_t              pwmSourceClockInHz;   //PWM时钟源
  pwm_config_t          pwmConfig;
  pwm_signal_param_t    pwmSignal[2];     //PWM子模块初始化结构体 
  
  pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);//时钟源  200MHz

  PWM_GetDefaultConfig(&pwmConfig);  //得到默认的PWM初始化结构体
  pwmConfig.reloadLogic       = kPWM_ReloadPwmFullCycle;   //循环输出
  pwmConfig.enableDebugMode   = true;    
  pwmConfig.prescale          = kPWM_Prescale_Divide_32;        //PWM时钟为 pwmSourceClockInHz 32分频 

  pwmSignal[0].pwmChannel       = kPWM_PwmA;        //默认使用通道A
  pwmSignal[0].level            = kPWM_HighTrue;    //输出电平为高电平
  pwmSignal[0].dutyCyclePercent = 0;                //初始占空比 0%
  pwmSignal[0].deadtimeValue    = 0;                //死区时间
  /*当AB两通道同时使用 才有作用*/
  pwmSignal[1].pwmChannel       = kPWM_PwmB;        // 使用PWMB
  pwmSignal[1].level            = kPWM_HighTrue;    //输出电平为高电平
  pwmSignal[1].dutyCyclePercent = 0;                //初始占空比 0%
  pwmSignal[1].deadtimeValue    = 0;                //死区时间

  
  PWM_Init(PWM2, kPWM_Module_0, &pwmConfig);
  PWM2->SM[kPWM_Module_0].DISMAP[0]=0;      //屏蔽故障检测功能 
  /*设置pwm的时钟 = pwmSourceClockInHz，频率 = Frequency 对齐方式 = kPWM_SignedCenterAligned，*/
  PWM_SetupPwm(PWM2, kPWM_Module_0, pwmSignal, 2, kPWM_SignedCenterAligned, 12000,pwmSourceClockInHz); 
  PWM_SetPwmLdok(PWM2, 1u<<kPWM_Module_0, true);    //设置pwm的 load ok位
  PWM_StartTimer(PWM2, 1u<<kPWM_Module_0);          //开启定时器
  
  PWM_Init(PWM1, kPWM_Module_3, &pwmConfig);
  PWM1->SM[kPWM_Module_3].DISMAP[0]=0;      //屏蔽故障检测功能
  PWM_SetupPwm(PWM1, kPWM_Module_3, pwmSignal, 2, kPWM_SignedCenterAligned, 12000,pwmSourceClockInHz); 
  PWM_SetPwmLdok(PWM1, 1u<<kPWM_Module_3, true);    //设置pwm的 load ok位
  PWM_StartTimer(PWM1, 1u<<kPWM_Module_3);          //开启定时器 
  
  PWM_Init(PWM2, kPWM_Module_3, &pwmConfig);
  PWM2->SM[kPWM_Module_3].DISMAP[0]=0;      //屏蔽故障检测功能
  PWM_SetupPwm(PWM2, kPWM_Module_3, pwmSignal, 1, kPWM_SignedCenterAligned, 200,pwmSourceClockInHz); 
  PWM_SetPwmLdok(PWM2, 1u<<kPWM_Module_3, true);    //设置pwm的 load ok位
  PWM_StartTimer(PWM2, 1u<<kPWM_Module_3);          //开启定时器 
  servo(3000);
}

void pwm_init(void)
{
  pwm_pinconfig();
  pwm_config();
}


/**
 *  舵机测试
 *  ----------------
 *  中值3000 频率200hz
 *  
 */
void servo_test(void)
{
  char txt[16];
  int servopwm = 0; 
  
  LCD_Init();
  LCD_CLS();
  key_init();          //按键及输入口初始化
  
  pwm_init();
  servo(3000);  //中值
  
  while (1)
  {    
    switch(key_read(0))  //检测按键
    {
    case 0:
      break;
    case 1:
      servopwm -= 10;
      servo(3000 + servopwm);//刷新servopwm频率
      break;           
    case 2:           
      servopwm = 0;
      servo(3000 + servopwm);//刷新servopwm频率
      break;
    case 3:           
      servopwm += 10;
      servo(3000 + servopwm);//刷新servopwm频率
      break;
    }
    sprintf(txt,"PWM: %4d",3000+servopwm);
    LCD_P6x8Str(0,0,(uint8_t*)txt); 

    LED_Color(red);     //红灯   
    delayms(50);
    LED_Color(blue);     //蓝灯   
    delayms(50);
  }
}

void Test_Motor(void)
{    
  char txt[16];
  short motorpwm=0;   
  LCD_Init();
  LCD_CLS();
  key_init();          //按键及输入口初始化
  
  pwm_init();

  while (1)
  {        
    switch(key_read(0))  //检测按键
    {
    case 0:
      break;
    case 1:
      motorpwm -= 100;
      if(motorpwm < -10000) motorpwm = -10000;
      left_motor(motorpwm);
      right_motor(motorpwm);
      break;           
    case 3:           
      motorpwm += 100;
      if(motorpwm > 10000) motorpwm = 10000;
      left_motor(motorpwm);
      right_motor(motorpwm);
      break;
    case 2:
      motorpwm = 0;
      left_motor(motorpwm);
      right_motor(motorpwm);
      break;
    }
    
    sprintf(txt,"PWM: %4.2f %",motorpwm/100.0f);
    LCD_P6x8Str(0,0,(uint8_t*)txt);
    
    //LED闪烁
    LED_Color(red);     //红灯   
    delayms(50);
    LED_Color(blue);     //红灯   
    delayms(50);
  }
}