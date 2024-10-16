static int klsi_105_prepare_write_buffer(struct usb_serial_port *port,
						void *dest, size_t size)
{
	unsigned char *buf = dest;
	int count;

	count = kfifo_out_locked(&port->write_fifo, buf + KLSI_HDR_LEN, size,
								&port->lock);
	put_unaligned_le16(count, buf);

	return count + KLSI_HDR_LEN;
}