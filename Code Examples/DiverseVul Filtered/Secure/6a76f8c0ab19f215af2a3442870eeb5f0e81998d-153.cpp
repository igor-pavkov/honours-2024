struct dyn_ftrace *ftrace_rec_iter_record(struct ftrace_rec_iter *iter)
{
	return &iter->pg->records[iter->index];
}