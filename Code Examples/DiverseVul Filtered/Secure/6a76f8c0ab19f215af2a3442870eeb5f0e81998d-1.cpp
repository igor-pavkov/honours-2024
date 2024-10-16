static int function_stat_cmp(void *p1, void *p2)
{
	struct ftrace_profile *a = p1;
	struct ftrace_profile *b = p2;

	if (a->counter < b->counter)
		return -1;
	if (a->counter > b->counter)
		return 1;
	else
		return 0;
}