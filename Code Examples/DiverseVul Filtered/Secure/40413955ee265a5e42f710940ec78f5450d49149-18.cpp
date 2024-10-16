int cipso_v4_sock_setattr(struct sock *sk,
			  const struct cipso_v4_doi *doi_def,
			  const struct netlbl_lsm_secattr *secattr)
{
	int ret_val = -EPERM;
	unsigned char *buf = NULL;
	u32 buf_len;
	u32 opt_len;
	struct ip_options_rcu *old, *opt = NULL;
	struct inet_sock *sk_inet;
	struct inet_connection_sock *sk_conn;

	/* In the case of sock_create_lite(), the sock->sk field is not
	 * defined yet but it is not a problem as the only users of these
	 * "lite" PF_INET sockets are functions which do an accept() call
	 * afterwards so we will label the socket as part of the accept(). */
	if (!sk)
		return 0;

	/* We allocate the maximum CIPSO option size here so we are probably
	 * being a little wasteful, but it makes our life _much_ easier later
	 * on and after all we are only talking about 40 bytes. */
	buf_len = CIPSO_V4_OPT_LEN_MAX;
	buf = kmalloc(buf_len, GFP_ATOMIC);
	if (!buf) {
		ret_val = -ENOMEM;
		goto socket_setattr_failure;
	}

	ret_val = cipso_v4_genopt(buf, buf_len, doi_def, secattr);
	if (ret_val < 0)
		goto socket_setattr_failure;
	buf_len = ret_val;

	/* We can't use ip_options_get() directly because it makes a call to
	 * ip_options_get_alloc() which allocates memory with GFP_KERNEL and
	 * we won't always have CAP_NET_RAW even though we _always_ want to
	 * set the IPOPT_CIPSO option. */
	opt_len = (buf_len + 3) & ~3;
	opt = kzalloc(sizeof(*opt) + opt_len, GFP_ATOMIC);
	if (!opt) {
		ret_val = -ENOMEM;
		goto socket_setattr_failure;
	}
	memcpy(opt->opt.__data, buf, buf_len);
	opt->opt.optlen = opt_len;
	opt->opt.cipso = sizeof(struct iphdr);
	kfree(buf);
	buf = NULL;

	sk_inet = inet_sk(sk);

	old = rcu_dereference_protected(sk_inet->inet_opt,
					lockdep_sock_is_held(sk));
	if (sk_inet->is_icsk) {
		sk_conn = inet_csk(sk);
		if (old)
			sk_conn->icsk_ext_hdr_len -= old->opt.optlen;
		sk_conn->icsk_ext_hdr_len += opt->opt.optlen;
		sk_conn->icsk_sync_mss(sk, sk_conn->icsk_pmtu_cookie);
	}
	rcu_assign_pointer(sk_inet->inet_opt, opt);
	if (old)
		kfree_rcu(old, rcu);

	return 0;

socket_setattr_failure:
	kfree(buf);
	kfree(opt);
	return ret_val;
}