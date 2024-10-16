static int selinux_socket_bind(struct socket *sock, struct sockaddr *address, int addrlen)
{
	struct sock *sk = sock->sk;
	u16 family;
	int err;

	err = sock_has_perm(current, sk, SOCKET__BIND);
	if (err)
		goto out;

	/*
	 * If PF_INET or PF_INET6, check name_bind permission for the port.
	 * Multiple address binding for SCTP is not supported yet: we just
	 * check the first address now.
	 */
	family = sk->sk_family;
	if (family == PF_INET || family == PF_INET6) {
		char *addrp;
		struct sk_security_struct *sksec = sk->sk_security;
		struct common_audit_data ad;
		struct lsm_network_audit net = {0,};
		struct sockaddr_in *addr4 = NULL;
		struct sockaddr_in6 *addr6 = NULL;
		unsigned short snum;
		u32 sid, node_perm;

		if (family == PF_INET) {
			addr4 = (struct sockaddr_in *)address;
			snum = ntohs(addr4->sin_port);
			addrp = (char *)&addr4->sin_addr.s_addr;
		} else {
			addr6 = (struct sockaddr_in6 *)address;
			snum = ntohs(addr6->sin6_port);
			addrp = (char *)&addr6->sin6_addr.s6_addr;
		}

		if (snum) {
			int low, high;

			inet_get_local_port_range(sock_net(sk), &low, &high);

			if (snum < max(PROT_SOCK, low) || snum > high) {
				err = sel_netport_sid(sk->sk_protocol,
						      snum, &sid);
				if (err)
					goto out;
				ad.type = LSM_AUDIT_DATA_NET;
				ad.u.net = &net;
				ad.u.net->sport = htons(snum);
				ad.u.net->family = family;
				err = avc_has_perm(sksec->sid, sid,
						   sksec->sclass,
						   SOCKET__NAME_BIND, &ad);
				if (err)
					goto out;
			}
		}

		switch (sksec->sclass) {
		case SECCLASS_TCP_SOCKET:
			node_perm = TCP_SOCKET__NODE_BIND;
			break;

		case SECCLASS_UDP_SOCKET:
			node_perm = UDP_SOCKET__NODE_BIND;
			break;

		case SECCLASS_DCCP_SOCKET:
			node_perm = DCCP_SOCKET__NODE_BIND;
			break;

		default:
			node_perm = RAWIP_SOCKET__NODE_BIND;
			break;
		}

		err = sel_netnode_sid(addrp, family, &sid);
		if (err)
			goto out;

		ad.type = LSM_AUDIT_DATA_NET;
		ad.u.net = &net;
		ad.u.net->sport = htons(snum);
		ad.u.net->family = family;

		if (family == PF_INET)
			ad.u.net->v4info.saddr = addr4->sin_addr.s_addr;
		else
			ad.u.net->v6info.saddr = addr6->sin6_addr;

		err = avc_has_perm(sksec->sid, sid,
				   sksec->sclass, node_perm, &ad);
		if (err)
			goto out;
	}
out:
	return err;
}