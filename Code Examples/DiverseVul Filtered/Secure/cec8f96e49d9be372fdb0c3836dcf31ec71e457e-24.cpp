int snd_timer_stop(struct snd_timer_instance *timeri)
{
	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
		return snd_timer_stop_slave(timeri, true);
	else
		return snd_timer_stop1(timeri, true);
}