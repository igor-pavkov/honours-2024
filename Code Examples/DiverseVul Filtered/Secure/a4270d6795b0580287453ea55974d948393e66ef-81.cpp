static inline void net_timestamp_set(struct sk_buff *skb)
{
	skb->tstamp = 0;
	if (static_branch_unlikely(&netstamp_needed_key))
		__net_timestamp(skb);
}