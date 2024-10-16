static int qdisc_dump_stab(struct sk_buff *skb, struct qdisc_size_table *stab)
{
	struct nlattr *nest;

	nest = nla_nest_start(skb, TCA_STAB);
	if (nest == NULL)
		goto nla_put_failure;
	NLA_PUT(skb, TCA_STAB_BASE, sizeof(stab->szopts), &stab->szopts);
	nla_nest_end(skb, nest);

	return skb->len;

nla_put_failure:
	return -1;
}