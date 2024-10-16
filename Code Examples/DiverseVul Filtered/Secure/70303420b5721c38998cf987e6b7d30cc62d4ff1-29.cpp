static int filter_pred_string(struct filter_pred *pred, void *event)
{
	char *addr = (char *)(event + pred->offset);
	int cmp, match;

	cmp = pred->regex.match(addr, &pred->regex, pred->regex.field_len);

	match = cmp ^ pred->not;

	return match;
}