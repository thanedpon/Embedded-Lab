#include "msp.h"
#include <stdio.h>
void UART0_init(void);
unsigned char UART0Rx(void);
int UART0Tx(unsigned char c);
void delay_ms (uint32_t delay);
int MCP3202 (int channel);
int channel = 0;
int MPC3202_data= 0;
int MPC3202_data2=0;
int ADC_data=0;
float Vout = 0;
int count_rx = 0;
uint8_t data_1 = 0, data_2 = 0, data_3 = 0;
int main (void) {
	UART0_init();
	EUSCI_B0 -> CTLW0 = 0x0001; //disable UCB0 during config
	EUSCI_B0 -> CTLW0 = 0x2BC1; //clock phase/polarity : 00, MSB first, 8bit, master, 4 pin SPI

	//sychronous mode,use SMCLK as clocksource

	EUSCI_B0 -> BRW = 30; //3MHz/ 30 = 100 kHz
	EUSCI_B0 -> CTLW0 &= ~0x0001; //enable UCB0 after config
	P1 -> SEL0 |= 0xE0; // P1.5 is CLK, P1.6 is SIMO, P1.7 is SOMI
	P1 -> SEL1 &= ~0xE0;
	P2 -> DIR |= 8; //P2.3 set as output for slave select ***CS at P2.3
	P2 -> OUT |= 8; //slave select idle hight
	///LED show status (blink)
	P2 -> SEL1 &= ~2; // configure P2.1 as simple I/O
	P2 -> SEL0 &= ~2;
	P2 -> DIR |= 2; // set P2.1 output
	
	TIMER_A1 -> CTL = 0x0211;
	TIMER_A1 -> EX0 = 0;
	TIMER_A1 -> CCR[0] = 30000-1; //1 ms 
	
	EUSCI_B0 -> IE |= 1; //enable resive interrupt
	NVIC_SetPriority(EUSCIB0_IRQn, 4); //set priority to 4 in NVIC
	NVIC_EnableIRQ(EUSCIB0_IRQn);// enable interrupt in NVIC
	__enable_irq();

	while (1) {
//		P2 -> OUT &= ~8; //assert slave select
//		while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty
//		EUSCI_B0 -> TXBUF = 0xF0; //write command
//		while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty
//		EUSCI_B0 -> TXBUF = 0x00; //write command
//		while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty
//		EUSCI_B0 -> TXBUF = 0x00; //write command
//		while (!(EUSCI_B0 ->IFG & 2)); //wait for transmit buffer emty
//		P2 -> OUT |= 8; //deassert slave select
//				ADC_data = (data_1 & 0x07)*512 + (data_2)*2 +(data_3 & 0x01);
//				Vout = (ADC_data * 3.3) / 4096;
//				printf("ADC High byte : %d\r\n",data_1 );	
//				printf("ADC Low byte : %d\r\n",data_2 );
//				printf("ADC Last bit : %d\r\n",(data_3 & 0x01) );
//				printf("ADC Value: %d\r\n",ADC_data);
//				printf("Vout: %d\r\n",Vout );
//				MCP3202(channel);
//				channel ^= 1;
//				delay_ms(1000);
//				P2->OUT ^= 2; //toggle green LED
		while((TIMER_A1 -> CCTL[0] & 1)==0);
		TIMER_A1 -> CCTL[0] &= ~1;
		MPC3202_data = MCP3202(0);
		MPC3202_data2 = MCP3202(1);
		//printf("%d\n", MPC3202_data);
//		printf("channel0: %f\n", 3.3 * MPC3202_data / 4095);
//	  printf("channel1 :%f\n", 3.3 * MPC3202_data2 / 4095);
//		delay_ms(10);	
		
}
}
///////////////////////Interrupt resive SPI ////////////////////////////
void EUSCIB0_IRQHandler (void) {
		if (count_rx == 0){
				data_1 = EUSCI_B0 -> RXBUF; //read theresive high byte data
				count_rx = count_rx + 1;
}
		else if (count_rx == 1){
				data_2 = EUSCI_B0 -> RXBUF; //read theresive high byte data
				count_rx = count_rx + 1;
}
		else if (count_rx == 2){
				data_3 = EUSCI_B0 -> RXBUF; //read theresive high byte data
				count_rx = 0;
}
}
void delay_ms (uint32_t delay){
uint32_t i;
SysTick -> LOAD = 3000-1; //Reload value
SysTick -> VAL = 0; //Clear current value
SysTick -> CTRL = 0x00000005; //Enable SysTick timer
for (i = 0; i < delay; i++)
{
while ((SysTick -> CTRL & 0x00010000) == 0) //Wait until the count flag is set
{ }
}
SysTick -> CTRL = 0; //stop timer
}

int MCP3202(int channel){
	P2 -> OUT &= ~8;
	while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty))
	if (channel == 0)
		EUSCI_B0 -> TXBUF = 0xD0;
	else if (channel == 1)
		EUSCI_B1 -> TXBUF = 0xF0;
	while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty
	EUSCI_B0 -> TXBUF = 0x00;		//write command
	while (!(EUSCI_B0 ->IFG & 2));//wait for transmit buffer emty
	EUSCI_B0 -> TXBUF = 0x00; //write command
	while (!(EUSCI_B0 ->IFG & 2)); //wait for transmit buffer emty
	P2 -> OUT |= 8; //deassert slave select
	ADC_data = (data_1 & 0x07)*512 + (data_2)*2 +(data_3 & 0x01);
	Vout = (ADC_data * 3.3) / 4095;
	printf("%f\n", Vout);
	printf("%f\n", Vout);
	delay_ms(10);	
	return ADC_data;
			//printf("ADC High byte : %d\r\n",data_1 );	
			//printf("ADC Low byte : %d\r\n",data_2 );
			//printf("ADC Last bit : %d\r\n",(data_3 & 0x01) );
			//printf("ADC Value: %d\r\n",ADC_data);
			//printf("Vout: %0.2f\r\n",Vout );
	
	
}

//////////////////////UART0_init////////////////////////////////////////
void UART0_init(void){
EUSCI_A0->CTLW0 |= 1; //put in reset mode for config
EUSCI_A0->MCTLW = 0; //disable oversampling
EUSCI_A0->CTLW0 = 0x0081; //1 stop bit, parity non, SMCK, 8 bits data
EUSCI_A0->BRW = 26; //baud rate 115200 (3000000/115200 = 26)
P1->SEL0 |= 0x0C; //P1.3 and P1.2 for UART
P1->SEL1 &= ~0x0C;
EUSCI_A0->CTLW0 &= ~1; //take UART out of reset mode
}
/////////////////////////////UART0Rx///////////////////////////////////
/* read a charater from UART */
unsigned char UART0Rx(void) {
char c;
while (!(EUSCI_A0 -> IFG & 0x01));
c = EUSCI_A0 -> RXBUF;
return c;}
/////////////////////////////UART0Tx///////////////////////////////////
/* write a charater to UART */
int UART0Tx (unsigned char c){
while (!(EUSCI_A0 -> IFG & 0x02));
EUSCI_A0 -> TXBUF = c;
return c;}
//////////////////////////////////////////////////////////////////////
/* The code below is the interface to the C standard I/O library
All the I/O are directed to the console, which is UART0.*/
struct __FILE { int handle;};
FILE __stdin = {0};
FILE __stdout = {1};
FILE __stderr = {2};
/*Called by by C library console/file input This function echoes the character received.
If the character is '\r', it is substituted by '\n'. */
int fgetc(FILE *f) {
int c;
c = UART0Rx(); // read the character from console
if (c == '\r') { //if '\r' replace with '\n'
UART0Tx(c); // echo
c = '\n'; }
UART0Tx(c); // echo
return c;}
//Called by C library console/file output
int fputc (int c, FILE *f) {
return UART0Tx(c);
}//write the character to console
