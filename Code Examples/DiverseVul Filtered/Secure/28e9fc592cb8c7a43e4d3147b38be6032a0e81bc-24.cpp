static int llc_ui_bind(struct socket *sock, struct sockaddr *uaddr, int addrlen)
{
	struct sockaddr_llc *addr = (struct sockaddr_llc *)uaddr;
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	struct llc_sap *sap;
	int rc = -EINVAL;

	dprintk("%s: binding %02X\n", __func__, addr->sllc_sap);
	if (unlikely(!sock_flag(sk, SOCK_ZAPPED) || addrlen != sizeof(*addr)))
		goto out;
	rc = -EAFNOSUPPORT;
	if (unlikely(addr->sllc_family != AF_LLC))
		goto out;
	rc = -ENODEV;
	rtnl_lock();
	llc->dev = dev_getbyhwaddr(&init_net, addr->sllc_arphrd, addr->sllc_mac);
	rtnl_unlock();
	if (!llc->dev)
		goto out;
	if (!addr->sllc_sap) {
		rc = -EUSERS;
		addr->sllc_sap = llc_ui_autoport();
		if (!addr->sllc_sap)
			goto out;
	}
	sap = llc_sap_find(addr->sllc_sap);
	if (!sap) {
		sap = llc_sap_open(addr->sllc_sap, NULL);
		rc = -EBUSY; /* some other network layer is using the sap */
		if (!sap)
			goto out;
		llc_sap_hold(sap);
	} else {
		struct llc_addr laddr, daddr;
		struct sock *ask;

		memset(&laddr, 0, sizeof(laddr));
		memset(&daddr, 0, sizeof(daddr));
		/*
		 * FIXME: check if the address is multicast,
		 * 	  only SOCK_DGRAM can do this.
		 */
		memcpy(laddr.mac, addr->sllc_mac, IFHWADDRLEN);
		laddr.lsap = addr->sllc_sap;
		rc = -EADDRINUSE; /* mac + sap clash. */
		ask = llc_lookup_established(sap, &daddr, &laddr);
		if (ask) {
			sock_put(ask);
			goto out_put;
		}
	}
	llc->laddr.lsap = addr->sllc_sap;
	memcpy(llc->laddr.mac, addr->sllc_mac, IFHWADDRLEN);
	memcpy(&llc->addr, addr, sizeof(llc->addr));
	/* assign new connection to its SAP */
	llc_sap_add_socket(sap, sk);
	sock_reset_flag(sk, SOCK_ZAPPED);
	rc = 0;
out_put:
	llc_sap_put(sap);
out:
	return rc;
}