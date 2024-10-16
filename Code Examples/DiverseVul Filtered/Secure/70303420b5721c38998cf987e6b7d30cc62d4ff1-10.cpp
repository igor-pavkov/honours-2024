static int ftrace_function_check_pred(struct filter_pred *pred)
{
	struct ftrace_event_field *field = pred->field;

	/*
	 * Check the predicate for function trace, verify:
	 *  - only '==' and '!=' is used
	 *  - the 'ip' field is used
	 */
	if ((pred->op != OP_EQ) && (pred->op != OP_NE))
		return -EINVAL;

	if (strcmp(field->name, "ip"))
		return -EINVAL;

	return 0;
}