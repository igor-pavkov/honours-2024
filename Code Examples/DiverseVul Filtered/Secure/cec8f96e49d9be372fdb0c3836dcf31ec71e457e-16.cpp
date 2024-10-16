static int snd_timer_s_close(struct snd_timer *timer)
{
	struct snd_timer_system_private *priv;

	priv = (struct snd_timer_system_private *)timer->private_data;
	del_timer_sync(&priv->tlist);
	return 0;
}