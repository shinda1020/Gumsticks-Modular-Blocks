#ifndef  __UART_GET_PUT_H__
#define  __UART_GET_PUT_H__

extern void uartInit(void);                 //  UART��ʼ��
extern void uartPutc(const char c);         //  ͨ��UART����һ���ַ�
extern void uartPuts(const char *s);        //  ͨ��UART�����ַ���
extern char uartGetc(void);                 //  ͨ��UART����һ���ַ�
extern int uartGets(char *s, int size);     //  ͨ��UART�����ַ��������ԣ�
                                            //  �˸�<Backspace>�޸ģ��س�<Enter>����

#endif  //  __UART_GET_PUT_H__
