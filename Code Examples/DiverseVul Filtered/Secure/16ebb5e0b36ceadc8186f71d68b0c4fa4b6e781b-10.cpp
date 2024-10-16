int tc_classify_compat(struct sk_buff *skb, struct tcf_proto *tp,
		       struct tcf_result *res)
{
	__be16 protocol = skb->protocol;
	int err = 0;

	for (; tp; tp = tp->next) {
		if ((tp->protocol == protocol ||
		     tp->protocol == htons(ETH_P_ALL)) &&
		    (err = tp->classify(skb, tp, res)) >= 0) {
#ifdef CONFIG_NET_CLS_ACT
			if (err != TC_ACT_RECLASSIFY && skb->tc_verd)
				skb->tc_verd = SET_TC_VERD(skb->tc_verd, 0);
#endif
			return err;
		}
	}
	return -1;
}