static void kd_nosound(struct timer_list *unused)
{
	static unsigned int zero;

	input_handler_for_each_handle(&kbd_handler, &zero, kd_sound_helper);
}