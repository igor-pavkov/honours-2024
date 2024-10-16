static _INLINE_ unsigned int read_option_register(struct mp_port *mtpt, int offset)
{
	return inb(mtpt->option_base_addr + offset);
}