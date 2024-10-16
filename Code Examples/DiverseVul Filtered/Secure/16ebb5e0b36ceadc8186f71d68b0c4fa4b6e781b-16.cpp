static int tc_dump_qdisc_root(struct Qdisc *root, struct sk_buff *skb,
			      struct netlink_callback *cb,
			      int *q_idx_p, int s_q_idx)
{
	int ret = 0, q_idx = *q_idx_p;
	struct Qdisc *q;

	if (!root)
		return 0;

	q = root;
	if (q_idx < s_q_idx) {
		q_idx++;
	} else {
		if (!tc_qdisc_dump_ignore(q) &&
		    tc_fill_qdisc(skb, q, q->parent, NETLINK_CB(cb->skb).pid,
				  cb->nlh->nlmsg_seq, NLM_F_MULTI, RTM_NEWQDISC) <= 0)
			goto done;
		q_idx++;
	}
	list_for_each_entry(q, &root->list, list) {
		if (q_idx < s_q_idx) {
			q_idx++;
			continue;
		}
		if (!tc_qdisc_dump_ignore(q) && 
		    tc_fill_qdisc(skb, q, q->parent, NETLINK_CB(cb->skb).pid,
				  cb->nlh->nlmsg_seq, NLM_F_MULTI, RTM_NEWQDISC) <= 0)
			goto done;
		q_idx++;
	}

out:
	*q_idx_p = q_idx;
	return ret;
done:
	ret = -1;
	goto out;
}