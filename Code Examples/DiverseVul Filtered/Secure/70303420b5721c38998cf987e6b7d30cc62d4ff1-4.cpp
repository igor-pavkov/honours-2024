static int filter_pred_strloc(struct filter_pred *pred, void *event)
{
	u32 str_item = *(u32 *)(event + pred->offset);
	int str_loc = str_item & 0xffff;
	int str_len = str_item >> 16;
	char *addr = (char *)(event + str_loc);
	int cmp, match;

	cmp = pred->regex.match(addr, &pred->regex, str_len);

	match = cmp ^ pred->not;

	return match;
}