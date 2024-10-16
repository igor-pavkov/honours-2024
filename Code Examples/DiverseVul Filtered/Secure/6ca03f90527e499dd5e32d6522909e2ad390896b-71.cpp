void kd_mksound(unsigned int hz, unsigned int ticks)
{
	del_timer_sync(&kd_mksound_timer);

	input_handler_for_each_handle(&kbd_handler, &hz, kd_sound_helper);

	if (hz && ticks)
		mod_timer(&kd_mksound_timer, jiffies + ticks);
}