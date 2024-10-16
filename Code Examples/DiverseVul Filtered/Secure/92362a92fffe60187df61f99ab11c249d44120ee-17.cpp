static void _launch_complete_wait(uint32_t job_id)
{
	int i, j, empty;
	time_t start = time(NULL);
	struct timeval now;
	struct timespec timeout;

	slurm_mutex_lock(&job_state_mutex);
	for (i = 0; ; i++) {
		empty = -1;
		for (j = 0; j < JOB_STATE_CNT; j++) {
			if (job_id == active_job_id[j])
				break;
			if ((active_job_id[j] == 0) && (empty == -1))
				empty = j;
		}
		if (j < JOB_STATE_CNT)	/* Found job, ready to return */
			break;
		if (difftime(time(NULL), start) <= 9) {  /* Retry for 9 secs */
			debug2("wait for launch of job %u before suspending it",
			       job_id);
			gettimeofday(&now, NULL);
			timeout.tv_sec  = now.tv_sec + 1;
			timeout.tv_nsec = now.tv_usec * 1000;
			pthread_cond_timedwait(&job_state_cond,&job_state_mutex,
					       &timeout);
			continue;
		}
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
		break;
	}
	slurm_mutex_unlock(&job_state_mutex);
	_launch_complete_log("job wait", job_id);
}