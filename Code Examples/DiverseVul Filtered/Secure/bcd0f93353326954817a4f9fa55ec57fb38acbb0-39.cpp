static int pep_accept_conn(struct sock *sk, struct sk_buff *skb)
{
	static const u8 data[20] = {
		PAD, PAD, PAD, 2 /* sub-blocks */,
		PN_PIPE_SB_REQUIRED_FC_TX, pep_sb_size(5), 3, PAD,
			PN_MULTI_CREDIT_FLOW_CONTROL,
			PN_ONE_CREDIT_FLOW_CONTROL,
			PN_LEGACY_FLOW_CONTROL,
			PAD,
		PN_PIPE_SB_PREFERRED_FC_RX, pep_sb_size(5), 3, PAD,
			PN_MULTI_CREDIT_FLOW_CONTROL,
			PN_ONE_CREDIT_FLOW_CONTROL,
			PN_LEGACY_FLOW_CONTROL,
			PAD,
	};

	might_sleep();
	return pep_reply(sk, skb, PN_PIPE_NO_ERROR, data, sizeof(data),
				GFP_KERNEL);
}