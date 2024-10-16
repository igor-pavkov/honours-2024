int snd_timer_start(struct snd_timer_instance *timeri, unsigned int ticks)
{
	if (timeri == NULL || ticks < 1)
		return -EINVAL;
	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
		return snd_timer_start_slave(timeri, true);
	else
		return snd_timer_start1(timeri, true, ticks);
}