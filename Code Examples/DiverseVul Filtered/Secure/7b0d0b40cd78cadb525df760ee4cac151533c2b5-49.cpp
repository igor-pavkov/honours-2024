static int selinux_socket_connect(struct socket *sock, struct sockaddr *address, int addrlen)
{
	struct sock *sk = sock->sk;
	struct sk_security_struct *sksec = sk->sk_security;
	int err;

	err = sock_has_perm(current, sk, SOCKET__CONNECT);
	if (err)
		return err;

	/*
	 * If a TCP or DCCP socket, check name_connect permission for the port.
	 */
	if (sksec->sclass == SECCLASS_TCP_SOCKET ||
	    sksec->sclass == SECCLASS_DCCP_SOCKET) {
		struct common_audit_data ad;
		struct lsm_network_audit net = {0,};
		struct sockaddr_in *addr4 = NULL;
		struct sockaddr_in6 *addr6 = NULL;
		unsigned short snum;
		u32 sid, perm;

		if (sk->sk_family == PF_INET) {
			addr4 = (struct sockaddr_in *)address;
			if (addrlen < sizeof(struct sockaddr_in))
				return -EINVAL;
			snum = ntohs(addr4->sin_port);
		} else {
			addr6 = (struct sockaddr_in6 *)address;
			if (addrlen < SIN6_LEN_RFC2133)
				return -EINVAL;
			snum = ntohs(addr6->sin6_port);
		}

		err = sel_netport_sid(sk->sk_protocol, snum, &sid);
		if (err)
			goto out;

		perm = (sksec->sclass == SECCLASS_TCP_SOCKET) ?
		       TCP_SOCKET__NAME_CONNECT : DCCP_SOCKET__NAME_CONNECT;

		ad.type = LSM_AUDIT_DATA_NET;
		ad.u.net = &net;
		ad.u.net->dport = htons(snum);
		ad.u.net->family = sk->sk_family;
		err = avc_has_perm(sksec->sid, sid, sksec->sclass, perm, &ad);
		if (err)
			goto out;
	}

	err = selinux_netlbl_socket_connect(sk, address);

out:
	return err;
}