void clock_was_set(void)
{
	/* Retrigger the CPU local events everywhere */
	on_each_cpu(retrigger_next_event, NULL, 0, 1);
}