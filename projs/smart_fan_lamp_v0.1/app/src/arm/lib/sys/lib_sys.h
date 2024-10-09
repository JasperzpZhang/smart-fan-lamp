

#ifndef __LIB_SYS_H__
#define __LIB_SYS_H__

#include "main.h"

/**
 * SYS_SUPPORT_OS���ڶ���ϵͳ�ļ����Ƿ�֧��OS
 * 0,��֧��OS
 * 1,֧��OS
 */
#define SYS_SUPPORT_OS          1


void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);             /* �����ж�ƫ���� */
void sys_standby(void);                                                         /* �������ģʽ */
void sys_soft_reset(void);                                                      /* ϵͳ��λ */
uint8_t sys_clock_set(uint32_t plln);                                           /* ʱ�����ú��� */
void sys_stm32_clock_init(uint32_t plln);                                       /* ϵͳʱ�ӳ�ʼ������ */

/* ����Ϊ��ຯ�� */
void sys_wfi_set(void);                                                         /* ִ��WFIָ�� */
void sys_intx_disable(void);                                                    /* �ر������ж� */
void sys_intx_enable(void);                                                     /* ���������ж� */
void sys_msr_msp(uint32_t addr);                                                /* ����ջ����ַ */

#endif /* __LIB_SYS_H__ */


