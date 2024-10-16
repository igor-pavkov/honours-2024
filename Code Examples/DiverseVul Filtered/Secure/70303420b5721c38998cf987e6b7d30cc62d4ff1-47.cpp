static bool is_or(struct prog_entry *prog, int i)
{
	int target;

	/*
	 * Only "||" is allowed for function events, thus,
	 * all true branches should jump to true, and any
	 * false branch should jump to false.
	 */
	target = prog[i].target + 1;
	/* True and false have NULL preds (all prog entries should jump to one */
	if (prog[target].pred)
		return false;

	/* prog[target].target is 1 for TRUE, 0 for FALSE */
	return prog[i].when_to_branch == prog[target].target;
}