static int filter_pred_comm(struct filter_pred *pred, void *event)
{
	int cmp;

	cmp = pred->regex.match(current->comm, &pred->regex,
				TASK_COMM_LEN);
	return cmp ^ pred->not;
}