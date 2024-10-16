static int ftrace_function_set_filter_pred(struct filter_pred *pred,
					   struct function_filter_data *data)
{
	int ret;

	/* Checking the node is valid for function trace. */
	ret = ftrace_function_check_pred(pred);
	if (ret)
		return ret;

	return __ftrace_function_set_filter(pred->op == OP_EQ,
					    pred->regex.pattern,
					    pred->regex.len,
					    data);
}