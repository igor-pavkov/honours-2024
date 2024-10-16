int __init kbd_init(void)
{
	int i;
	int error;

	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		kbd_table[i].ledflagstate = kbd_defleds();
		kbd_table[i].default_ledflagstate = kbd_defleds();
		kbd_table[i].ledmode = LED_SHOW_FLAGS;
		kbd_table[i].lockstate = KBD_DEFLOCK;
		kbd_table[i].slockstate = 0;
		kbd_table[i].modeflags = KBD_DEFMODE;
		kbd_table[i].kbdmode = default_utf8 ? VC_UNICODE : VC_XLATE;
	}

	kbd_init_leds();

	error = input_register_handler(&kbd_handler);
	if (error)
		return error;

	tasklet_enable(&keyboard_tasklet);
	tasklet_schedule(&keyboard_tasklet);

	return 0;
}