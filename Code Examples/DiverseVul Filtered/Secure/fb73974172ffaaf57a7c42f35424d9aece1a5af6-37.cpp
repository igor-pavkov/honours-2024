static void selinux_sctp_sk_clone(struct sctp_endpoint *ep, struct sock *sk,
				  struct sock *newsk)
{
	struct sk_security_struct *sksec = sk->sk_security;
	struct sk_security_struct *newsksec = newsk->sk_security;

	/* If policy does not support SECCLASS_SCTP_SOCKET then call
	 * the non-sctp clone version.
	 */
	if (!selinux_policycap_extsockclass())
		return selinux_sk_clone_security(sk, newsk);

	newsksec->sid = ep->secid;
	newsksec->peer_sid = ep->peer_secid;
	newsksec->sclass = sksec->sclass;
	selinux_netlbl_sctp_sk_clone(sk, newsk);
}