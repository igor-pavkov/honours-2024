static void _remove_job_running_prolog(uint32_t job_id)
{
	ListIterator iter;
	uint32_t *job_running_prolog;
	bool found = false;

	slurm_mutex_lock(&conf->prolog_running_lock);

	iter = list_iterator_create(conf->prolog_running_jobs);
	while ((job_running_prolog = list_next(iter))) {
		if (*job_running_prolog  == job_id) {
			job_running_prolog = list_remove(iter);
			xfree(job_running_prolog);

			found = true;
			pthread_cond_broadcast(&conf->prolog_running_cond);
			break;
		}
	}
	if (!found)
		error("_remove_job_running_prolog: job not found");

	slurm_mutex_unlock(&conf->prolog_running_lock);
}