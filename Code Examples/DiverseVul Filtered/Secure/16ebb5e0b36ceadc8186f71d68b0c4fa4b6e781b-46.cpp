static int tc_fill_qdisc(struct sk_buff *skb, struct Qdisc *q, u32 clid,
			 u32 pid, u32 seq, u16 flags, int event)
{
	struct tcmsg *tcm;
	struct nlmsghdr  *nlh;
	unsigned char *b = skb_tail_pointer(skb);
	struct gnet_dump d;

	nlh = NLMSG_NEW(skb, pid, seq, event, sizeof(*tcm), flags);
	tcm = NLMSG_DATA(nlh);
	tcm->tcm_family = AF_UNSPEC;
	tcm->tcm__pad1 = 0;
	tcm->tcm__pad2 = 0;
	tcm->tcm_ifindex = qdisc_dev(q)->ifindex;
	tcm->tcm_parent = clid;
	tcm->tcm_handle = q->handle;
	tcm->tcm_info = atomic_read(&q->refcnt);
	NLA_PUT_STRING(skb, TCA_KIND, q->ops->id);
	if (q->ops->dump && q->ops->dump(q, skb) < 0)
		goto nla_put_failure;
	q->qstats.qlen = q->q.qlen;

	if (q->stab && qdisc_dump_stab(skb, q->stab) < 0)
		goto nla_put_failure;

	if (gnet_stats_start_copy_compat(skb, TCA_STATS2, TCA_STATS, TCA_XSTATS,
					 qdisc_root_sleeping_lock(q), &d) < 0)
		goto nla_put_failure;

	if (q->ops->dump_stats && q->ops->dump_stats(q, &d) < 0)
		goto nla_put_failure;

	if (gnet_stats_copy_basic(&d, &q->bstats) < 0 ||
	    gnet_stats_copy_rate_est(&d, &q->rate_est) < 0 ||
	    gnet_stats_copy_queue(&d, &q->qstats) < 0)
		goto nla_put_failure;

	if (gnet_stats_finish_copy(&d) < 0)
		goto nla_put_failure;

	nlh->nlmsg_len = skb_tail_pointer(skb) - b;
	return skb->len;

nlmsg_failure:
nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}