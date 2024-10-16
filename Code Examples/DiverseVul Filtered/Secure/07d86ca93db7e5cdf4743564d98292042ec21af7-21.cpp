static void dump_urb(const char *type, const u8 *data, int length)
{
	snd_printk(KERN_DEBUG "%s packet: [", type);
	for (; length > 0; ++data, --length)
		printk(" %02x", *data);
	printk(" ]\n");
}