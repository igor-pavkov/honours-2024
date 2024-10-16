struct ftrace_rec_iter *ftrace_rec_iter_next(struct ftrace_rec_iter *iter)
{
	iter->index++;

	if (iter->index >= iter->pg->index) {
		iter->pg = iter->pg->next;
		iter->index = 0;

		/* Could have empty pages */
		while (iter->pg && !iter->pg->index)
			iter->pg = iter->pg->next;
	}

	if (!iter->pg)
		return NULL;

	return iter;
}