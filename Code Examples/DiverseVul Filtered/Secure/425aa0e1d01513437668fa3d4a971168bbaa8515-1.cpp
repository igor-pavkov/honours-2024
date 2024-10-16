int compat_ip_getsockopt(struct sock *sk, int level, int optname,
			 char __user *optval, int __user *optlen)
{
	int err;

	if (optname == MCAST_MSFILTER)
		return compat_mc_getsockopt(sk, level, optname, optval, optlen,
			ip_getsockopt);

	err = do_ip_getsockopt(sk, level, optname, optval, optlen,
		MSG_CMSG_COMPAT);

#if IS_ENABLED(CONFIG_BPFILTER_UMH)
	if (optname >= BPFILTER_IPT_SO_GET_INFO &&
	    optname < BPFILTER_IPT_GET_MAX)
		err = bpfilter_ip_get_sockopt(sk, optname, optval, optlen);
#endif
#ifdef CONFIG_NETFILTER
	/* we need to exclude all possible ENOPROTOOPTs except default case */
	if (err == -ENOPROTOOPT && optname != IP_PKTOPTIONS &&
			!ip_mroute_opt(optname)) {
		int len;

		if (get_user(len, optlen))
			return -EFAULT;

		err = compat_nf_getsockopt(sk, PF_INET, optname, optval, &len);
		if (err >= 0)
			err = put_user(len, optlen);
		return err;
	}
#endif
	return err;
}