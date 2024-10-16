static int sb1053a_get_interface(struct mp_port *mtpt, int port_num)
{
	unsigned long option_base_addr = mtpt->option_base_addr;
	unsigned int  interface = 0;

	switch (port_num)
	{
		case 0:
		case 1:
			/* set GPO[1:0] = 00 */
			outb(0x00, option_base_addr + MP_OPTR_GPODR);
			break;
		case 2:
		case 3:
			/* set GPO[1:0] = 01 */
			outb(0x01, option_base_addr + MP_OPTR_GPODR);
			break;
		case 4:
		case 5:
			/* set GPO[1:0] = 10 */
			outb(0x02, option_base_addr + MP_OPTR_GPODR);
			break;
		default:
			break;
	}

	port_num &= 0x1;

	/* get interface */
	interface = inb(option_base_addr + MP_OPTR_IIR0 + port_num);

	/* set GPO[1:0] = 11 */
	outb(0x03, option_base_addr + MP_OPTR_GPODR);

	return (interface);
}