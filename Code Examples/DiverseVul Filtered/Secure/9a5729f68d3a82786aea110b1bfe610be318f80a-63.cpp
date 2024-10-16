int sisusb_copy_memory(struct sisusb_usb_data *sisusb, char *src,
		u32 dest, int length)
{
	size_t dummy;

	return sisusb_write_mem_bulk(sisusb, dest, src, length,
			NULL, 0, &dummy);
}