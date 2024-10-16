static inline unsigned char getleds(void)
{
	struct kbd_struct *kb = kbd_table + fg_console;

	if (kb->ledmode == LED_SHOW_IOCTL)
		return ledioctl;

	return kb->ledflagstate;
}