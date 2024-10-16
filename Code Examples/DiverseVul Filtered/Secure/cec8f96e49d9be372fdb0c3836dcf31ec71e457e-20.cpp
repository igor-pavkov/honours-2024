int snd_timer_continue(struct snd_timer_instance *timeri)
{
	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
		return snd_timer_start_slave(timeri, false);
	else
		return snd_timer_start1(timeri, false, 0);
}