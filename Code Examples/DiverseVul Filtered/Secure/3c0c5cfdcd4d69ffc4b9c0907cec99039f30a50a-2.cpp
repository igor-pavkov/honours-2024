static int pvc_bind(struct socket *sock, struct sockaddr *sockaddr,
		    int sockaddr_len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_atmpvc *addr;
	struct atm_vcc *vcc;
	int error;

	if (sockaddr_len != sizeof(struct sockaddr_atmpvc))
		return -EINVAL;
	addr = (struct sockaddr_atmpvc *)sockaddr;
	if (addr->sap_family != AF_ATMPVC)
		return -EAFNOSUPPORT;
	lock_sock(sk);
	vcc = ATM_SD(sock);
	if (!test_bit(ATM_VF_HASQOS, &vcc->flags)) {
		error = -EBADFD;
		goto out;
	}
	if (test_bit(ATM_VF_PARTIAL, &vcc->flags)) {
		if (vcc->vpi != ATM_VPI_UNSPEC)
			addr->sap_addr.vpi = vcc->vpi;
		if (vcc->vci != ATM_VCI_UNSPEC)
			addr->sap_addr.vci = vcc->vci;
	}
	error = vcc_connect(sock, addr->sap_addr.itf, addr->sap_addr.vpi,
			    addr->sap_addr.vci);
out:
	release_sock(sk);
	return error;
}