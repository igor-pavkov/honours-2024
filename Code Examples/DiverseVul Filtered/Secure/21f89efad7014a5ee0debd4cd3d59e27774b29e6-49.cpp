inline static void _add_priority(priority_st * st, const int *list)
{
	int num, i, j, init;

	init = i = st->algorithms;

	for (num = 0; list[num] != 0; ++num) {
		if (i + 1 > MAX_ALGOS) {
			return;
		}

		for (j = 0; j < init; j++) {
			if (st->priority[j] == (unsigned) list[num]) {
				break;
			}
		}

		if (j == init) {
			st->priority[i++] = list[num];
			st->algorithms++;
		}
	}

	return;
}