static int test_pred_visited_fn(struct filter_pred *pred, void *event)
{
	struct ftrace_event_field *field = pred->field;

	test_pred_visited = 1;
	printk(KERN_INFO "\npred visited %s\n", field->name);
	return 1;
}