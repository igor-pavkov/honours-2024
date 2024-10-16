static void update_preds(struct prog_entry *prog, int N, int invert)
{
	int t, s;

	t = prog[N].target;
	s = prog[t].target;
	prog[t].when_to_branch = invert;
	prog[t].target = N;
	prog[N].target = s;
}