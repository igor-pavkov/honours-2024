static int selinux_socket_unix_stream_connect(struct sock *sock,
					      struct sock *other,
					      struct sock *newsk)
{
	struct sk_security_struct *sksec_sock = sock->sk_security;
	struct sk_security_struct *sksec_other = other->sk_security;
	struct sk_security_struct *sksec_new = newsk->sk_security;
	struct common_audit_data ad;
	struct lsm_network_audit net = {0,};
	int err;

	ad.type = LSM_AUDIT_DATA_NET;
	ad.u.net = &net;
	ad.u.net->sk = other;

	err = avc_has_perm(sksec_sock->sid, sksec_other->sid,
			   sksec_other->sclass,
			   UNIX_STREAM_SOCKET__CONNECTTO, &ad);
	if (err)
		return err;

	/* server child socket */
	sksec_new->peer_sid = sksec_sock->sid;
	err = security_sid_mls_copy(sksec_other->sid, sksec_sock->sid,
				    &sksec_new->sid);
	if (err)
		return err;

	/* connecting socket */
	sksec_sock->peer_sid = sksec_new->sid;

	return 0;
}