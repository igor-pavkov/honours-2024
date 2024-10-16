int ftrace_test_record(struct dyn_ftrace *rec, int enable)
{
	return ftrace_check_record(rec, enable, 0);
}