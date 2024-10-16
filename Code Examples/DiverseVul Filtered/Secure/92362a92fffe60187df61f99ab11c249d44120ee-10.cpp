static void _fb_wrlock(void)
{
	slurm_mutex_lock(&file_bcast_mutex);
	fb_write_wait_lock++;
	while (1) {
		if ((fb_read_lock == 0) && (fb_write_lock == 0)) {
			fb_write_lock++;
			fb_write_wait_lock--;
			break;
		} else {	/* wait for state change and retry */
			pthread_cond_wait(&file_bcast_cond, &file_bcast_mutex);
		}
	}
	slurm_mutex_unlock(&file_bcast_mutex);
}