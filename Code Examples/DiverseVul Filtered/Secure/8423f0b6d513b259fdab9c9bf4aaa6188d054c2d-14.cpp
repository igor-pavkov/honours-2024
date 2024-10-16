static void unlock_params(struct snd_pcm_runtime *runtime)
{
	mutex_unlock(&runtime->oss.params_lock);
}