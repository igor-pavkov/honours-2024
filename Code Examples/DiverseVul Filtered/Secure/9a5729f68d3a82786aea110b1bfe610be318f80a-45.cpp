static void sisusb_testreadwrite(struct sisusb_usb_data *sisusb)
{
	static char srcbuffer[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
	char destbuffer[10];
	int i, j;

	sisusb_copy_memory(sisusb, srcbuffer, sisusb->vrambase, 7);

	for (i = 1; i <= 7; i++) {
		dev_dbg(&sisusb->sisusb_dev->dev,
				"sisusb: rwtest %d bytes\n", i);
		sisusb_read_memory(sisusb, destbuffer, sisusb->vrambase, i);
		for (j = 0; j < i; j++) {
			dev_dbg(&sisusb->sisusb_dev->dev,
					"rwtest read[%d] = %x\n",
					j, destbuffer[j]);
		}
	}
}