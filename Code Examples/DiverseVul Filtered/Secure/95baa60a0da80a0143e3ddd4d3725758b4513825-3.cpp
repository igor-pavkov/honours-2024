int compat_ipv6_setsockopt(struct sock *sk, int level, int optname,
			   char __user *optval, unsigned int optlen)
{
	int err;

	if (level == SOL_IP && sk->sk_type != SOCK_RAW) {
		if (udp_prot.compat_setsockopt != NULL)
			return udp_prot.compat_setsockopt(sk, level, optname,
							  optval, optlen);
		return udp_prot.setsockopt(sk, level, optname, optval, optlen);
	}

	if (level != SOL_IPV6)
		return -ENOPROTOOPT;

	if (optname >= MCAST_JOIN_GROUP && optname <= MCAST_MSFILTER)
		return compat_mc_setsockopt(sk, level, optname, optval, optlen,
			ipv6_setsockopt);

	err = do_ipv6_setsockopt(sk, level, optname, optval, optlen);
#ifdef CONFIG_NETFILTER
	/* we need to exclude all possible ENOPROTOOPTs except default case */
	if (err == -ENOPROTOOPT && optname != IPV6_IPSEC_POLICY &&
	    optname != IPV6_XFRM_POLICY)
		err = compat_nf_setsockopt(sk, PF_INET6, optname, optval,
					   optlen);
#endif
	return err;
}