#include "atk_usart.h"
#include "atk_delay.h"
 
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/9/7
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
//////////////////////////////////////////////////////////////////////////////////       
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB      
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)    

#if 1

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 
};


FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
    x = x; 
} 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{     
    while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
    return ch;
}

//__use_no_semihosting was requested, but _ttywrch was 
//void _ttywrch(int ch)
//{
//    ch = ch;
//}


#endif 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���       
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��    ������ɱ�־
//bit14��    ���յ�0x0d
//bit13~0��    ���յ�����Ч�ֽ���Ŀ

static u16 USART_RX_STA=0;       //����״̬���    

UART_HandleTypeDef UART1_Handler; //UART���

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART1_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_Initure;
    
    if(huart->Instance==USART1)                //����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();          //ʹ��GPIOAʱ��
        __HAL_RCC_USART1_CLK_ENABLE();         //ʹ��USART1ʱ��
    
        GPIO_Initure.Pin=GPIO_PIN_9;            //PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //��©��
        GPIO_Initure.Pull=GPIO_NOPULL;          //����
        GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
        GPIO_Initure.Alternate=GPIO_AF7_USART1; //����ΪUSART1
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //��ʼ��PA9

        GPIO_Initure.Pin=GPIO_PIN_10;            //PA10
        HAL_GPIO_Init(GPIOA,&GPIO_Initure);      //��ʼ��PA10
    
    }

}

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound)
{            
    //UART ��ʼ������
    UART1_Handler.Instance=USART1;                        //USART1
    UART1_Handler.Init.BaudRate=bound;                    //������
    UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
    UART1_Handler.Init.StopBits=UART_STOPBITS_1;        //һ��ֹͣλ
    UART1_Handler.Init.Parity=UART_PARITY_NONE;            //����żУ��λ
    UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
    UART1_Handler.Init.Mode=UART_MODE_TX_RX;            //�շ�ģʽ
    HAL_UART1_MspInit(&UART1_Handler);   

    if (HAL_UART_Init(&UART1_Handler) != HAL_OK)
    {
        while(1);
    }
    __HAL_UART_DISABLE_IT(&UART1_Handler,UART_IT_TXE);   //��ֹ�����ж�     
    __HAL_UART_DISABLE_IT(&UART1_Handler,UART_IT_TC);    //���ܷ�������ж�
    
#if EN_USART1_RX
        __HAL_UART_ENABLE_IT(&UART1_Handler,UART_IT_RXNE);//���������ж�
        HAL_NVIC_EnableIRQ(USART1_IRQn);                //ʹ��USART1�ж�ͨ��
        HAL_NVIC_SetPriority(USART1_IRQn,5,0);            //��ռ���ȼ�5�������ȼ�0
#endif        
}



//����1�жϷ������
void USART1_IRQHandler(void)                    
{ 
    u8 Res;
    if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        HAL_UART_Receive(&UART1_Handler,&Res,1,1000); 
        if((USART_RX_STA&0x8000)==0)//����δ���
        {
            if(USART_RX_STA&0x4000)//���յ���0x0d
            {
                if(Res!=0x0a)USART_RX_STA=0;  //���մ���,���¿�ʼ
                else USART_RX_STA|=0x8000;    //��������� 
            }
            else //��û�յ�0X0D
            {    
                if(Res==0x0d)USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����      
                }         
            }
        }            
    }
    HAL_UART_IRQHandler(&UART1_Handler);    
} 
#endif    


