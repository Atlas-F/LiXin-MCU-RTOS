/**
 *
 * @file   adc_dma_sampling.h
 * @brief
 *
 * @author AtlasFg
 * @date   2026-08-02
 * @version 1.0
 *
 * ********************************************************************************
 */

#ifndef _ADC_DMA_SAMPLING_H_
#define _ADC_DMA_SAMPLING_H_

BaseType_t AppADC_DMA_Init(void);
BaseType_t Switch_DMA_BufferTarget(void);
BaseType_t SetDMATargetTask(void *pvparameters);
BaseType_t DataConversionTask(void *pvparameters);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif _ADC_DMA_SAMPLING_H_
