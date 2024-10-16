static unsigned int selinux_ip_output(struct sk_buff *skb,
				      u16 family)
{
	struct sock *sk;
	u32 sid;

	if (!netlbl_enabled())
		return NF_ACCEPT;

	/* we do this in the LOCAL_OUT path and not the POST_ROUTING path
	 * because we want to make sure we apply the necessary labeling
	 * before IPsec is applied so we can leverage AH protection */
	sk = skb->sk;
	if (sk) {
		struct sk_security_struct *sksec;

		if (sk_listener(sk))
			/* if the socket is the listening state then this
			 * packet is a SYN-ACK packet which means it needs to
			 * be labeled based on the connection/request_sock and
			 * not the parent socket.  unfortunately, we can't
			 * lookup the request_sock yet as it isn't queued on
			 * the parent socket until after the SYN-ACK is sent.
			 * the "solution" is to simply pass the packet as-is
			 * as any IP option based labeling should be copied
			 * from the initial connection request (in the IP
			 * layer).  it is far from ideal, but until we get a
			 * security label in the packet itself this is the
			 * best we can do. */
			return NF_ACCEPT;

		/* standard practice, label using the parent socket */
		sksec = sk->sk_security;
		sid = sksec->sid;
	} else
		sid = SECINITSID_KERNEL;
	if (selinux_netlbl_skbuff_setsid(skb, family, sid) != 0)
		return NF_DROP;

	return NF_ACCEPT;
}