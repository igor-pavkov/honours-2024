static _INLINE_ void serial_out(struct mp_port *mtpt, int offset, int value)
{
	outb(value, mtpt->port.iobase + offset);
}