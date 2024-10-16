static int _wait_for_starting_step(uint32_t job_id, uint32_t step_id)
{
	starting_step_t  starting_step;
	starting_step.job_id  = job_id;
	starting_step.step_id = step_id;
	int num_passes = 0;

	slurm_mutex_lock(&conf->starting_steps_lock);

	while (list_find_first( conf->starting_steps,
				&_compare_starting_steps,
				&starting_step )) {
		if (num_passes == 0) {
			if (step_id != NO_VAL)
				debug( "Blocked waiting for step %d.%d",
					job_id, step_id);
			else
				debug( "Blocked waiting for job %d, all steps",
					job_id);
		}
		num_passes++;

		pthread_cond_wait(&conf->starting_steps_cond,
				  &conf->starting_steps_lock);
	}
	if (num_passes > 0) {
		if (step_id != NO_VAL)
			debug( "Finished wait for step %d.%d",
				job_id, step_id);
		else
			debug( "Finished wait for job %d, all steps",
				job_id);
	}
	slurm_mutex_unlock(&conf->starting_steps_lock);

	return SLURM_SUCCESS;
}