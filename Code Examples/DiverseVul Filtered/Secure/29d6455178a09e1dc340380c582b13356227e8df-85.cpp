bool single_task_running(void)
{
	return raw_rq()->nr_running == 1;
}