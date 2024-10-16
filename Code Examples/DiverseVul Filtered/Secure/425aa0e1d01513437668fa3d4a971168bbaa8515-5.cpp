int ip_setsockopt(struct sock *sk, int level,
		int optname, char __user *optval, unsigned int optlen)
{
	int err;

	if (level != SOL_IP)
		return -ENOPROTOOPT;

	err = do_ip_setsockopt(sk, level, optname, optval, optlen);
#if IS_ENABLED(CONFIG_BPFILTER_UMH)
	if (optname >= BPFILTER_IPT_SO_SET_REPLACE &&
	    optname < BPFILTER_IPT_SET_MAX)
		err = bpfilter_ip_set_sockopt(sk, optname, optval, optlen);
#endif
#ifdef CONFIG_NETFILTER
	/* we need to exclude all possible ENOPROTOOPTs except default case */
	if (err == -ENOPROTOOPT && optname != IP_HDRINCL &&
			optname != IP_IPSEC_POLICY &&
			optname != IP_XFRM_POLICY &&
			!ip_mroute_opt(optname))
		err = nf_setsockopt(sk, PF_INET, optname, optval, optlen);
#endif
	return err;
}