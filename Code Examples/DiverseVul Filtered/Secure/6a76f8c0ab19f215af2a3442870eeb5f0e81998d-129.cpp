void __weak ftrace_replace_code(int enable)
{
	struct dyn_ftrace *rec;
	struct ftrace_page *pg;
	int failed;

	if (unlikely(ftrace_disabled))
		return;

	do_for_each_ftrace_rec(pg, rec) {
		failed = __ftrace_replace_code(rec, enable);
		if (failed) {
			ftrace_bug(failed, rec->ip);
			/* Stop processing */
			return;
		}
	} while_for_each_ftrace_rec();
}