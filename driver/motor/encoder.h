#ifndef _ENCODER_H
#define _ENCODER_H

/**
 *  初始化ENC1、ENC3
 *  ----------------
 *  使用ENC_GetPositionDifferenceValue(ENC1)函数读取计数值
 *  
 */
void enc_init(void);

/**
 *  EN11、ENC3测试
 *  ----------------
 *  
 */
void encoder_test(void);

#endif 