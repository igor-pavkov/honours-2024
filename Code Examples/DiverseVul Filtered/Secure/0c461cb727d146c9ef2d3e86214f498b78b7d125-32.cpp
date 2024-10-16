static unsigned int selinux_ip_postroute_compat(struct sk_buff *skb,
						int ifindex,
						u16 family)
{
	struct sock *sk = skb_to_full_sk(skb);
	struct sk_security_struct *sksec;
	struct common_audit_data ad;
	struct lsm_network_audit net = {0,};
	char *addrp;
	u8 proto;

	if (sk == NULL)
		return NF_ACCEPT;
	sksec = sk->sk_security;

	ad.type = LSM_AUDIT_DATA_NET;
	ad.u.net = &net;
	ad.u.net->netif = ifindex;
	ad.u.net->family = family;
	if (selinux_parse_skb(skb, &ad, &addrp, 0, &proto))
		return NF_DROP;

	if (selinux_secmark_enabled())
		if (avc_has_perm(sksec->sid, skb->secmark,
				 SECCLASS_PACKET, PACKET__SEND, &ad))
			return NF_DROP_ERR(-ECONNREFUSED);

	if (selinux_xfrm_postroute_last(sksec->sid, skb, &ad, proto))
		return NF_DROP_ERR(-ECONNREFUSED);

	return NF_ACCEPT;
}