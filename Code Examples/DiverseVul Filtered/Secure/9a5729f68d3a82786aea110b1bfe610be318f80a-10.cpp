static int sisusb_read_memory(struct sisusb_usb_data *sisusb, char *dest,
		u32 src, int length)
{
	size_t dummy;

	return sisusb_read_mem_bulk(sisusb, src, dest, length,
			NULL, &dummy);
}