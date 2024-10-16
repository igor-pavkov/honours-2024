inline static void _set_priority(priority_st * st, const int *list)
{
	int num = 0, i;

	while (list[num] != 0)
		num++;
	if (num > MAX_ALGOS)
		num = MAX_ALGOS;
	st->algorithms = num;

	for (i = 0; i < num; i++) {
		st->priority[i] = list[i];
	}

	return;
}