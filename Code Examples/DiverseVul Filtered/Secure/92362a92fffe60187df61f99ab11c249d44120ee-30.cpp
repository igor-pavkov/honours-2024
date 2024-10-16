static int _compare_starting_steps(void *listentry, void *key)
{
	starting_step_t *step0 = (starting_step_t *)listentry;
	starting_step_t *step1 = (starting_step_t *)key;

	if (step1->step_id != NO_VAL)
		return (step0->job_id  == step1->job_id &&
			step0->step_id == step1->step_id);
	else
		return (step0->job_id  == step1->job_id);
}