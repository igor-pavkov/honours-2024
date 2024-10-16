static void line6_wait_clear_audio_urbs(struct snd_line6_pcm *line6pcm,
					struct line6_pcm_stream *pcms)
{
	int timeout = HZ;
	int i;
	int alive;

	do {
		alive = 0;
		for (i = 0; i < line6pcm->line6->iso_buffers; i++) {
			if (test_bit(i, &pcms->active_urbs))
				alive++;
		}
		if (!alive)
			break;
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(1);
	} while (--timeout > 0);
	if (alive)
		dev_err(line6pcm->line6->ifcdev,
			"timeout: still %d active urbs..\n", alive);
}