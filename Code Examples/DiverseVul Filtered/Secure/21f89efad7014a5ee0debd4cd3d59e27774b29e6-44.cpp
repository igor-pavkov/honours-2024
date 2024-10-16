static void prio_remove(priority_st * priority_list, unsigned int algo)
{
	unsigned int i;

	for (i = 0; i < priority_list->algorithms; i++) {
		if (priority_list->priority[i] == algo) {
			priority_list->algorithms--;
			if ((priority_list->algorithms - i) > 0)
				memmove(&priority_list->priority[i],
					&priority_list->priority[i + 1],
					(priority_list->algorithms -
					 i) *
					sizeof(priority_list->
					       priority[0]));
			priority_list->priority[priority_list->
						algorithms] = 0;
			break;
		}
	}

	return;
}