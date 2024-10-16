static int sock_has_perm(struct sock *sk, u32 perms)
{
	struct sk_security_struct *sksec = sk->sk_security;
	struct common_audit_data ad;
	struct lsm_network_audit net = {0,};

	if (sksec->sid == SECINITSID_KERNEL)
		return 0;

	ad.type = LSM_AUDIT_DATA_NET;
	ad.u.net = &net;
	ad.u.net->sk = sk;

	return avc_has_perm(&selinux_state,
			    current_sid(), sksec->sid, sksec->sclass, perms,
			    &ad);
}