static int filter_pred_pchar(struct filter_pred *pred, void *event)
{
	char **addr = (char **)(event + pred->offset);
	int cmp, match;
	int len = strlen(*addr) + 1;	/* including tailing '\0' */

	cmp = pred->regex.match(*addr, &pred->regex, len);

	match = cmp ^ pred->not;

	return match;
}