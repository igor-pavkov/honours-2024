static void _launch_complete_add(uint32_t job_id)
{
	int j, empty;

	slurm_mutex_lock(&job_state_mutex);
	empty = -1;
	for (j = 0; j < JOB_STATE_CNT; j++) {
		if (job_id == active_job_id[j])
			break;
		if ((active_job_id[j] == 0) && (empty == -1))
			empty = j;
	}
	if (j >= JOB_STATE_CNT || job_id != active_job_id[j]) {
		if (empty == -1)	/* Discard oldest job */
			empty = 0;
		for (j = empty + 1; j < JOB_STATE_CNT; j++) {
			active_job_id[j - 1] = active_job_id[j];
		}
		active_job_id[JOB_STATE_CNT - 1] = 0;
		for (j = 0; j < JOB_STATE_CNT; j++) {
			if (active_job_id[j] == 0) {
				active_job_id[j] = job_id;
				break;
			}
		}
	}
	pthread_cond_signal(&job_state_cond);
	slurm_mutex_unlock(&job_state_mutex);
	_launch_complete_log("job add", job_id);
}