int snd_timer_pause(struct snd_timer_instance * timeri)
{
	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
		return snd_timer_stop_slave(timeri, false);
	else
		return snd_timer_stop1(timeri, false);
}