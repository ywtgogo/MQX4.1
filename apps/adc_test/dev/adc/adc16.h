/**HEADER********************************************************************
* 
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: adc16.h$
* $Version : 3.8.35.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains ADC16 module driver.
*
*END************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

#include <mqx.h>
#include <bsp.h>
#include "common.h"

#define ADC0     ADC0_BASE_PTR
#define ADC1     ADC1_BASE_PTR

#define IS_ADC_ALL_PERIPH(PERIPH) (((PERIPH) == ADC0) || ((PERIPH) == ADC1))

typedef enum
{
    ADC_DIFF_DISABLE = 0x00, //���ʹ�ܹر�
    ADC_DIFF_ENABLE  = 0x01  //���ʹ�ܿ���
}ADC_DIFF;

#define IS_ADC_DIFF(DIFF) (((DIFF) == ADC_DIFF_DISABLE) || ((DIFF) == ADC_DIFF_ENABLE))

typedef enum
{
    ADC_CHANNEL_DP0        = 0x00,
    ADC_CHANNEL_DP1        = 0x01,
    ADC_CHANNEL_DP2        = 0x02,
    ADC_CHANNEL_DP3        = 0x03,
    ADC_CHANNEL_DP0_DIFF   = 0x20,
    ADC_CHANNEL_DP1_DIFF   = 0x21,
    ADC_CHANNEL_DP2_DIFF   = 0x22,
    ADC_CHANNEL_DP3_DIFF   = 0x23,
    ADC_CHANNEL_SE4x       = 0x04,
    ADC_CHANNEL_SE5x       = 0x05,
    ADC_CHANNEL_SE6x       = 0x06,
    ADC_CHANNEL_SE7x       = 0x07,
    ADC_CHANNEL_SE8        = 0x08,
    ADC_CHANNEL_SE9        = 0x09,
    ADC_CHANNEL_SE10       = 0x0A,
    ADC_CHANNEL_SE11       = 0x0B,
    ADC_CHANNEL_SE12       = 0x0C,
    ADC_CHANNEL_SE13       = 0x0D,
    ADC_CHANNEL_SE14       = 0x0E,
    ADC_CHANNEL_SE15       = 0x0F,
    ADC_CHANNEL_SE16       = 0x10,
    ADC_CHANNEL_SE17       = 0x11,
    ADC_CHANNEL_SE18       = 0x12,
    ADC_CHANNEL_DM0        = 0x13,
    ADC_CHANNEL_DM1        = 0x14,
    ADC_CHANNEL_SE21       = 0x15,
    ADC_CHANNEL_SE22       = 0x16,
    ADC_CHANNEL_DAC        = 0x17,
    ADC_CHANNEL_TEMPSENEOR = 0x1A,
    ADC_CHANNEL_BANDGAP    = 0x1B,
    ADC_CHANNEL_VREFSH     = 0x1D,
    ADC_CHANNEL_VREFSL     = 0x1E,
    ADC_CHANNEL_DISABLE    = 0x1F
}ADC_CHANNEL;
#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_CHANNEL_DP0) || ((CHANNEL) == ADC_CHANNEL_DP1) ||\
                                 ((CHANNEL) == ADC_CHANNEL_PGA) || ((CHANNEL) == ADC_CHANNEL_DP3) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE4x) || ((CHANNEL) == ADC_CHANNEL_SE5x) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE6x) || ((CHANNEL) == ADC_CHANNEL_SE7x) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE8) || ((CHANNEL) == ADC_CHANNEL_SE9) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE10) || ((CHANNEL) == ADC_CHANNEL_SE11) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE12) || ((CHANNEL) == ADC_CHANNEL_SE13) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE14) || ((CHANNEL) == ADC_CHANNEL_SE15) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE16) || ((CHANNEL) == ADC_CHANNEL_SE17) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE18) || ((CHANNEL) == ADC_CHANNEL_DM0) ||\
                                 ((CHANNEL) == ADC_CHANNEL_DM1) || ((CHANNEL) == ADC_CHANNEL_DAC) ||\
                                 ((CHANNEL) == ADC_CHANNEL_TEMPSENEOR) || ((CHANNEL) == ADC_CHANNEL_BANDGAP) ||\
                                 ((CHANNEL) == ADC_CHANNEL_VREFSH) || ((CHANNEL) == ADC_CHANNEL_VREFSL) ||\
                                 ((CHANNEL) == ADC_CHANNEL_DISABLE))

typedef enum
{
    ADC_LOWPOWER_DISABLE = 0x00, //�͵�ѹ���ùر�
    ADC_LOWPOWER_ENABLE  = 0x01  //�͵�ѹ���ÿ���
}ADC_LOWPOWER;
#define IS_ADC_LOWPOWER(LOWPOWER) (((LOWPOWER) == ADC_LOWPOWER_DISABLE) || ((LOWPOWER) == ADC_LOWPOWER_ENABLE))

typedef enum
{
    ADC_CLOCK_DIVSION_1 = 0x00, //ADCʱ�Ӳ���Ƶ
    ADC_CLOCK_DIVSION_2 = 0x01, //ADCʱ��2��Ƶ
    ADC_CLOCK_DIVSION_4 = 0x02, //ADCʱ��4��Ƶ
    ADC_CLOCK_DIVSION_8 = 0x03, //ADCʱ��8��Ƶ
}ADC_CLOCK_DIVSION;
#define IS_ADC_CLOCK_DIVSION(DIVSION) (((DIVSION) == ADC_CLOCK_DIVSION_1) || ((DIVSION) == ADC_CLOCK_DIVSION_2) ||\
                                       ((DIVSION) == ADC_CLOCK_DIVSION_4) || ((DIVSION) == ADC_CLOCK_DIVSION_8))

typedef enum
{
    ADC_LONG_SAMPLE_DISABLE = 0x00, //�������ر�
    ADC_LONG_SAMPLE_ENABLE  = 0x01  //����������
}ADC_LONG_SAMPLE;
#define IS_ADC_LONG_SAMPLE(SAMPLE) (((SAMPLE) == ADC_LONG_SAMPLE_DISABLE) || ((SAMPLE) == ADC_LONG_SAMPLE_ENABLE))

typedef enum
{
    ADC_MODE_8_BIT  = 0x00, //8λ����
    ADC_MODE_12_BIT = 0x01, //12λ����
    ADC_MODE_10_BIT = 0x02, //10λ����
    ADC_MODE_16_BIT = 0x03  //16λ����
}ADC_MODE;
#define IS_ADC_MODE(MODE) (((MODE) == ADC_MODE_8_BIT) || ((MODE) == ADC_MODE_12_BIT) ||\
                           ((MODE) == ADC_MODE_10_BIT) || ((MODE) == ADC_MODE_16_BIT))

typedef enum
{
    ADC_CLOCK_BUSCLK      = 0x00, //����ʱ��
    ADC_CLOCK_BUSCLK_DIV2 = 0x01, //����ʱ��/2
    ADC_CLOCK_ALTCLK      = 0x02, //����ʱ��
    ADC_CLOCK_ADACK       = 0x03  //�첽ʱ��
}ADC_CLOCK;
#define IS_ADC_CLOCK(CLOCK) (((CLOCK) == ADC_CLOCK_BUSCLK) || ((CLOCK) == ADC_CLOCK_BUSCLK_DIV2) ||\
                             ((CLOCK) == ADC_CLOCK_ALTCLK) || ((CLOCK) == ADC_CLOCK_ADACK))

typedef enum
{
    ADC_MUXSEL_ADxxa = 0x00, //����a
    ADC_MUXSEL_ADxxb = 0x01  //����b
}ADC_MUXSEL;
#define IS_ADC_MUXSEL(MUXSEL) (((MUXSEL) == ADC_MUXSEL_ADxxa) || ((MUXSEL) == ADC_MUXSEL_ADxxb))

typedef enum
{
    ADC_ADACK_DISABLE = 0x00, //ȡ���첽ʱ��
    ADC_ADACK_ENABLE  = 0x01  //�����첽ʱ��
}ADC_ADACK;
#define IS_ADC_ADACK(ADACK) (((ADACK) == ADC_ADACK_DISABLE) || ((ADACK) == ADC_ADACK_ENABLE))

typedef enum
{
    ADC_SPEED_NORMAL = 0x00, //����ת������
    ADC_SPEED_HIGH   = 0x01  //����ת������
}ADC_SPEED;
#define IS_ADC_SPEED(SPEED) (((SPEED) == ADC_SPEED_NORMAL) || ((SPEED) == ADC_SPEED_HIGH))

typedef enum
{
    ADC_LONG_SAMPLE_TIME_20ADCK = 0x00, //����20��ADCK����
    ADC_LONG_SAMPLE_TIME_12ADCK = 0x01, //����12��ADCK����
    ADC_LONG_SAMPLE_TIME_6ADCK  = 0x02, //����6��ADCK����
    ADC_LONG_SAMPLE_TIME_2ADCK  = 0x03  //����2��ADCK����
}ADC_LONG_SAMPLE_TIME;
#define IS_ADC_LONG_SAMPLE_TIME(TIME) (((TIME) == ADC_LONG_SAMPLE_TIME_20ADCK) || ((TIME) == ADC_LONG_SAMPLE_TIME_12ADCK) ||\
                                       ((TIME) == ADC_LONG_SAMPLE_TIME_6ADCK) || ((TIME) == ADC_LONG_SAMPLE_TIME_2ADCK))

typedef enum
{
    ADC_TRIGGER_SOFTWARE = 0x00, //�������ת��
    ADC_TRIGGER_HARDWARE = 0x01  //Ӳ������ת��
}ADC_TRIGGER;
#define IS_ADC_TRIGGER(TRIGGER) (((TRIGGER) == ADC_TRIGGER_SOFTWARE) || ((TRIGGER) == ADC_TRIGGER_HARDWARE))

typedef enum
{
    ADC_COMPARE_DISABLE = 0x00, //�ȽϹ��ܹر�
    ADC_COMPARE_ENABLE  = 0x01  //�ȽϹ��ܿ���
}ADC_COMPARE;
#define IS_ADC_COMPARE(COMPARE) (((COMPARE) == ADC_COMPARE_DISABLE) || ((COMPARE) == ADC_COMPARE_ENABLE))

typedef enum
{
    ADC_REFSEL_EXTERNAL = 0x00, //�ο��ⲿ��ѹ
    ADC_REFSEL_INTERNAL = 0x01  //�ο��ڲ���ѹ
}ADC_REFSEL;
#define IS_ADC_REFSEL(REFSEL) (((REFSEL) == ADC_REFSEL_EXTERNAL) || ((REFSEL) == ADC_REFSEL_INTERNAL))

typedef enum
{
    ADC_AVERAGE_DISABLE = 0x00, //Ӳ����ֵ�ر�
    ADC_AVERAGE_ENABLE  = 0x01  //Ӳ����ֵ����
}ADC_AVERAGE;
#define IS_ADC_AVERAGE(AVERAGE) (((AVERAGE) == ADC_AVERAGE_DISABLE) || ((AVERAGE) == ADC_AVERAGE_ENABLE))

typedef enum
{
    ADC_AVERAGE_SELECT_4    = 0x00, //4������ֵȡƽ��
    ADC_AVERAGE_SELECT_8    = 0x01, //8������ֵȡƽ��
    ADC_AVERAGE_SELECT_16   = 0x02, //16������ֵȡƽ��
    ADC_AVERAGE_SELECT_32   = 0x03  //32������ֵȡƽ��
}ADC_AVERAGE_SELECT;
#define IS_ADC_AVERAGE_SELECT(SELECT) (((SELECT) == ADC_AVERAGE_SELECT_4) || ((SELECT) == ADC_AVERAGE_SELECT_8) ||\
                                       ((SELECT) == ADC_AVERAGE_SELECT_16) || ((SELECT) == ADC_AVERAGE_SELECT_32))

typedef enum
{
    ADC_AVERAGE_CONTINUOUS_1  = 0x00, //��ֵ�ڼ�ֻ��1��ת��
    ADC_AVERAGE_CONTINUOUS_X  = 0x01  //��ֵ�ڼ���ת��
}ADC_AVERAGE_CONTINUOUS;
#define IS_ADC_AVERAGE_CONTINUOUS(CONTINUOUS) (((CONTINUOUS) == ADC_AVERAGE_CONTINUOUS_1) || ((CONTINUOUS) == ADC_AVERAGE_CONTINUOUS_X))

typedef enum
{
    ADC_PGA_DISABLE = 0x00, //�̿طŴ����ر�
    ADC_PGA_ENABLE  = 0x01  //�̿طŴ�������
}ADC_PGA;
#define IS_ADC_PGA(PGA) (((PGA) == ADC_PGA_DISABLE) || ((PGA) == ADC_PGA_ENABLE))

typedef enum
{
    ADC_PGA_POWER_LOW     = 0x00, //�̿طŴ����ڵ͹���������
    ADC_PGA_POWER_NORMAL  = 0x01  //�̿طŴ�������������������
}ADC_PGA_POWER;
#define IS_ADC_PGA_POWER(POWER) (((POWER) == ADC_PGA_POWER_LOW) || ((POWER) == ADC_PGA_POWER_NORMAL))

typedef enum
{
    ADC_PGA_GAIN_1  = 0x00, //�Ŵ���
    ADC_PGA_GAIN_2  = 0x01,
    ADC_PGA_GAIN_4  = 0x02,
    ADC_PGA_GAIN_8  = 0x03,
    ADC_PGA_GAIN_16 = 0x04,
    ADC_PGA_GAIN_32 = 0x05,
    ADC_PGA_GAIN_64 = 0x06
}ADC_PGA_GAIN;
#define IS_ADC_PGA_GAIN(GAIN) (((GAIN) == ADC_PGA_GAIN_1) || ((GAIN) == ADC_PGA_GAIN_2) ||\
                               ((GAIN) == ADC_PGA_GAIN_4) || ((GAIN) == ADC_PGA_GAIN_8) ||\
                               ((GAIN) == ADC_PGA_GAIN_16) || ((GAIN) == ADC_PGA_GAIN_32) ||\
                               ((GAIN) == ADC_PGA_GAIN_64))

typedef struct
{
    ADC_CLOCK               xAdcClock;              //ʱ��
    ADC_CLOCK_DIVSION       xAdcClockDivsion;       //ʱ�ӷ�Ƶ
    ADC_ADACK               xAdcAck;                //�첽ʱ�����
    ADC_DIFF                xAdcDiff;               //���ʹ��
    ADC_LOWPOWER            xAdcLowpower;           //�͹���ģʽ
    ADC_MODE                xAdcMode;               //��������
    ADC_LONG_SAMPLE         xAdcLongSample;         //������
    ADC_LONG_SAMPLE_TIME    xAdcLongSampleTime;     //������ʱ��
    ADC_SPEED               xAdcSpeed;              //ת���ٶ�
    ADC_TRIGGER             xAdcTrigger;            //������ʽ
    ADC_REFSEL              xAdcRefsel;             //�ο���ѹ
    ADC_AVERAGE             xAdcAverage;            //Ӳ����ֵ
    ADC_AVERAGE_SELECT      xAdcAverageSelect;      //Ӳ����ֵ��������
    ADC_AVERAGE_CONTINUOUS  xAdcAverageContinuous;  //Ӳ����ֵ����ת������
	void                    (*old_isr)(pointer);    //�жϷ���������
	pointer                 old_isr_data;           //�жϷ���������
}ADC_PARAMETER;

typedef struct
{
    ADC_PGA_POWER           xAdcPgaPower;           //���л���
    ADC_PGA_GAIN            xAdcPgaGain;            //�Ŵ���
}ADC_PGA_PARAMETER;

#define ADC_PGA_PGLPB_MASK                        0x100000u


/* Initialization and Configuration functions *********************************/
void ADC_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter);
uint8_t ADC_Cal(ADC_MemMapPtr pxAdcMoudelNumber);
void ADC_StructInit(ADC_PARAMETER* pxAdcParameter);
void ADC_DMACmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState);
void ADC_ITConfig(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter, FUNCTIONALSTATE xNewState);
void ADC_MuxCmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_MUXSEL xAdcMux);
void ADC_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_CHANNEL xChannel);
#if 0
void ADC_PGA_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PGA_PARAMETER* pxAdcPgaParameter);
void ADC_PGA_StructInit(ADC_PGA_PARAMETER* pxAdcPgaParameter);
void ADC_PGA_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState);
#endif

/* Read and Write functions **********************************************/
uint16_t ADC_GetDataResult(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter);
FLAGSTATUS ADC_GetFlagConversionProgress(ADC_MemMapPtr pxAdcMoudelNumber);
FLAGSTATUS ADC_GetFlagStatus(ADC_MemMapPtr pxAdcMoudelNumber);
//void ADC_ClearFlag(ADC_MemMapPtr pxAdcMoudelNumber); ��ȡADֵ����дͨ��ֵ�����������־λ��

/* ADC ISRs */
void adc0_isr(pointer isr);
void adc1_isr(pointer isr);

#endif  /* __ADC_H */

