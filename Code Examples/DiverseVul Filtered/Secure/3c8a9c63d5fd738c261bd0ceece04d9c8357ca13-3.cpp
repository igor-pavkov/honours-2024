static int check_filter(struct tap_filter *filter, const struct sk_buff *skb)
{
	if (!filter->count)
		return 1;

	return run_filter(filter, skb);
}