static void _add_job_running_prolog(uint32_t job_id)
{
	uint32_t *job_running_prolog;

	/* Add the job to a list of jobs whose prologs are running */
	slurm_mutex_lock(&conf->prolog_running_lock);
	job_running_prolog = xmalloc(sizeof(uint32_t));
	if (!job_running_prolog) {
		error("_add_job_running_prolog failed to allocate memory");
		goto fail;
	}

	*job_running_prolog = job_id;
	if (!list_append(conf->prolog_running_jobs, job_running_prolog)) {
		error("_add_job_running_prolog failed to append job to list");
		xfree(job_running_prolog);
	}

fail:
	slurm_mutex_unlock(&conf->prolog_running_lock);
}