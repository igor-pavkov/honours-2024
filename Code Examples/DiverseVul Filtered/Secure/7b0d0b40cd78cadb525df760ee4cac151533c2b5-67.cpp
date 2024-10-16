static int selinux_socket_unix_may_send(struct socket *sock,
					struct socket *other)
{
	struct sk_security_struct *ssec = sock->sk->sk_security;
	struct sk_security_struct *osec = other->sk->sk_security;
	struct common_audit_data ad;
	struct lsm_network_audit net = {0,};

	ad.type = LSM_AUDIT_DATA_NET;
	ad.u.net = &net;
	ad.u.net->sk = other->sk;

	return avc_has_perm(ssec->sid, osec->sid, osec->sclass, SOCKET__SENDTO,
			    &ad);
}