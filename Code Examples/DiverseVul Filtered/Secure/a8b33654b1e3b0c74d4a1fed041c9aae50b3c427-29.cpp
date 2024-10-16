static void SendATCommand(struct mp_port *mtpt)
{
	//		      a    t	cr   lf
	unsigned char ch[] = {0x61,0x74,0x0d,0x0a,0x0};
	unsigned char lineControl;
	unsigned char i=0;
	unsigned char Divisor = 0xc;

	lineControl = serial_inp(mtpt,UART_LCR);
	serial_outp(mtpt,UART_LCR,(lineControl | UART_LCR_DLAB));
	serial_outp(mtpt,UART_DLL,(Divisor & 0xff));
	serial_outp(mtpt,UART_DLM,(Divisor & 0xff00)>>8); //baudrate is 4800


	serial_outp(mtpt,UART_LCR,lineControl);	
	serial_outp(mtpt,UART_LCR,0x03); // N-8-1
	serial_outp(mtpt,UART_FCR,7); 
	serial_outp(mtpt,UART_MCR,0x3);
	while(ch[i]){
		while((serial_inp(mtpt,UART_LSR) & 0x60) !=0x60){
			;
		}
		serial_outp(mtpt,0,ch[i++]);
	}


}// end of SendATCommand()