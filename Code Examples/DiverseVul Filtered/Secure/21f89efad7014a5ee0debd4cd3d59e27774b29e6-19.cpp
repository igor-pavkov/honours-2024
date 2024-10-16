static void _clear_given_priorities(priority_st * st, const int *list)
{
	unsigned i;

	for (i=0;list[i]!=0;i++) {
		prio_remove(st, list[i]);
	}
}