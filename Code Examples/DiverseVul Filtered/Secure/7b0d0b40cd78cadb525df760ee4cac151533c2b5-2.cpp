static int selinux_tun_dev_attach(struct sock *sk, void *security)
{
	struct tun_security_struct *tunsec = security;
	struct sk_security_struct *sksec = sk->sk_security;

	/* we don't currently perform any NetLabel based labeling here and it
	 * isn't clear that we would want to do so anyway; while we could apply
	 * labeling without the support of the TUN user the resulting labeled
	 * traffic from the other end of the connection would almost certainly
	 * cause confusion to the TUN user that had no idea network labeling
	 * protocols were being used */

	sksec->sid = tunsec->sid;
	sksec->sclass = SECCLASS_TUN_SOCKET;

	return 0;
}