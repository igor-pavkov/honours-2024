static void free_prog(struct event_filter *filter)
{
	struct prog_entry *prog;
	int i;

	prog = rcu_access_pointer(filter->prog);
	if (!prog)
		return;

	for (i = 0; prog[i].pred; i++)
		kfree(prog[i].pred);
	kfree(prog);
}