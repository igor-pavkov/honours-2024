static _INLINE_ unsigned int serial_in(struct mp_port *mtpt, int offset)
{
	return inb(mtpt->port.iobase + offset);
}