int compat_ip_setsockopt(struct sock *sk, int level, int optname,
			 char __user *optval, unsigned int optlen)
{
	int err;

	if (level != SOL_IP)
		return -ENOPROTOOPT;

	if (optname >= MCAST_JOIN_GROUP && optname <= MCAST_MSFILTER)
		return compat_mc_setsockopt(sk, level, optname, optval, optlen,
			ip_setsockopt);

	err = do_ip_setsockopt(sk, level, optname, optval, optlen);
#ifdef CONFIG_NETFILTER
	/* we need to exclude all possible ENOPROTOOPTs except default case */
	if (err == -ENOPROTOOPT && optname != IP_HDRINCL &&
			optname != IP_IPSEC_POLICY &&
			optname != IP_XFRM_POLICY &&
			!ip_mroute_opt(optname))
		err = compat_nf_setsockopt(sk, PF_INET, optname, optval,
					   optlen);
#endif
	return err;
}