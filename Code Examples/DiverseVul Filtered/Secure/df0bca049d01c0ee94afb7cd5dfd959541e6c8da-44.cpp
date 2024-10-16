static void sock_copy(struct sock *nsk, const struct sock *osk)
{
#ifdef CONFIG_SECURITY_NETWORK
	void *sptr = nsk->sk_security;
#endif

	memcpy(nsk, osk, osk->sk_prot->obj_size);
#ifdef CONFIG_SECURITY_NETWORK
	nsk->sk_security = sptr;
	security_sk_clone(osk, nsk);
#endif
}