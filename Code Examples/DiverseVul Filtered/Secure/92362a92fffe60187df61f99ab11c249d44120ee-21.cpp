static void _fb_wrunlock(void)
{
	slurm_mutex_lock(&file_bcast_mutex);
	fb_write_lock--;
	pthread_cond_broadcast(&file_bcast_cond);
	slurm_mutex_unlock(&file_bcast_mutex);
}