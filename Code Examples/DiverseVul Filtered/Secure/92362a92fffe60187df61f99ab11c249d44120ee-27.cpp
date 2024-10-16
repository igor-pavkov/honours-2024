static void *_prolog_timer(void *x)
{
	int delay_time, rc = SLURM_SUCCESS;
	struct timespec ts;
	struct timeval now;
	slurm_msg_t msg;
	job_notify_msg_t notify_req;
	char srun_msg[128];
	timer_struct_t *timer_struct = (timer_struct_t *) x;

	delay_time = MAX(2, (timer_struct->msg_timeout - 2));
	gettimeofday(&now, NULL);
	ts.tv_sec = now.tv_sec + delay_time;
	ts.tv_nsec = now.tv_usec * 1000;
	slurm_mutex_lock(timer_struct->timer_mutex);
	if (!timer_struct->prolog_fini) {
		rc = pthread_cond_timedwait(timer_struct->timer_cond,
					    timer_struct->timer_mutex,
					    &ts);
	}
	slurm_mutex_unlock(timer_struct->timer_mutex);

	if (rc != ETIMEDOUT)
		return NULL;

	slurm_msg_t_init(&msg);
	snprintf(srun_msg, sizeof(srun_msg), "Prolog hung on node %s",
		 conf->node_name);
	notify_req.job_id	= timer_struct->job_id;
	notify_req.job_step_id	= NO_VAL;
	notify_req.message	= srun_msg;
	msg.msg_type	= REQUEST_JOB_NOTIFY;
	msg.data	= &notify_req;
	slurm_send_only_controller_msg(&msg);
	return NULL;
}