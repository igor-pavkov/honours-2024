static void _wait_for_job_running_prolog(uint32_t job_id)
{
	debug( "Waiting for job %d's prolog to complete", job_id);
	slurm_mutex_lock(&conf->prolog_running_lock);

	while (_prolog_is_running (job_id)) {
		pthread_cond_wait(&conf->prolog_running_cond,
				  &conf->prolog_running_lock);
	}

	slurm_mutex_unlock(&conf->prolog_running_lock);
	debug( "Finished wait for job %d's prolog to complete", job_id);
}