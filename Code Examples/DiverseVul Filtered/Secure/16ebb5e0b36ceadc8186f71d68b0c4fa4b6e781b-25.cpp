int tc_classify(struct sk_buff *skb, struct tcf_proto *tp,
		struct tcf_result *res)
{
	int err = 0;
	__be16 protocol;
#ifdef CONFIG_NET_CLS_ACT
	struct tcf_proto *otp = tp;
reclassify:
#endif
	protocol = skb->protocol;

	err = tc_classify_compat(skb, tp, res);
#ifdef CONFIG_NET_CLS_ACT
	if (err == TC_ACT_RECLASSIFY) {
		u32 verd = G_TC_VERD(skb->tc_verd);
		tp = otp;

		if (verd++ >= MAX_REC_LOOP) {
			printk("rule prio %u protocol %02x reclassify loop, "
			       "packet dropped\n",
			       tp->prio&0xffff, ntohs(tp->protocol));
			return TC_ACT_SHOT;
		}
		skb->tc_verd = SET_TC_VERD(skb->tc_verd, verd);
		goto reclassify;
	}
#endif
	return err;
}