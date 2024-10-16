static void _fb_rdunlock(void)
{
	slurm_mutex_lock(&file_bcast_mutex);
	fb_read_lock--;
	pthread_cond_broadcast(&file_bcast_cond);
	slurm_mutex_unlock(&file_bcast_mutex);
}