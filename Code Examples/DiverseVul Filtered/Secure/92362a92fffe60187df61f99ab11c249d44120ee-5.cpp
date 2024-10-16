static bool _step_is_starting(uint32_t job_id, uint32_t step_id)
{
	starting_step_t  starting_step;
	starting_step.job_id  = job_id;
	starting_step.step_id = step_id;
	bool ret = false;

	slurm_mutex_lock(&conf->starting_steps_lock);

	if (list_find_first( conf->starting_steps,
			     &_compare_starting_steps,
			     &starting_step )) {
		ret = true;
	}

	slurm_mutex_unlock(&conf->starting_steps_lock);
	return ret;
}