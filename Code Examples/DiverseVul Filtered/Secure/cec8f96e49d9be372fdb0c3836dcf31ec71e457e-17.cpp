static void snd_timer_user_disconnect(struct snd_timer_instance *timeri)
{
	struct snd_timer_user *tu = timeri->callback_data;

	tu->disconnected = true;
	wake_up(&tu->qchange_sleep);
}