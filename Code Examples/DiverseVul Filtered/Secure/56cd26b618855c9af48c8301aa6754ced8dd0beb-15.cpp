static void send_pulse_irdeo(unsigned int length, ktime_t target)
{
	long rawbits;
	int i;
	unsigned char output;
	unsigned char chunk, shifted;

	/* how many bits have to be sent ? */
	rawbits = length * 1152 / 10000;
	if (serial_ir.duty_cycle > 50)
		chunk = 3;
	else
		chunk = 1;
	for (i = 0, output = 0x7f; rawbits > 0; rawbits -= 3) {
		shifted = chunk << (i * 3);
		shifted >>= 1;
		output &= (~shifted);
		i++;
		if (i == 3) {
			soutp(UART_TX, output);
			while (!(sinp(UART_LSR) & UART_LSR_THRE))
				;
			output = 0x7f;
			i = 0;
		}
	}
	if (i != 0) {
		soutp(UART_TX, output);
		while (!(sinp(UART_LSR) & UART_LSR_TEMT))
			;
	}
}