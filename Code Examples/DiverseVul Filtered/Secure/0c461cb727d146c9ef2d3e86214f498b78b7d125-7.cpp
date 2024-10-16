static int selinux_sk_alloc_security(struct sock *sk, int family, gfp_t priority)
{
	struct sk_security_struct *sksec;

	sksec = kzalloc(sizeof(*sksec), priority);
	if (!sksec)
		return -ENOMEM;

	sksec->peer_sid = SECINITSID_UNLABELED;
	sksec->sid = SECINITSID_UNLABELED;
	sksec->sclass = SECCLASS_SOCKET;
	selinux_netlbl_sk_security_reset(sksec);
	sk->sk_security = sksec;

	return 0;
}