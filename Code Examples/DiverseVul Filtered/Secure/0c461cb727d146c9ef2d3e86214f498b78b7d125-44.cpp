static int selinux_socket_getpeersec_dgram(struct socket *sock, struct sk_buff *skb, u32 *secid)
{
	u32 peer_secid = SECSID_NULL;
	u16 family;
	struct inode_security_struct *isec;

	if (skb && skb->protocol == htons(ETH_P_IP))
		family = PF_INET;
	else if (skb && skb->protocol == htons(ETH_P_IPV6))
		family = PF_INET6;
	else if (sock)
		family = sock->sk->sk_family;
	else
		goto out;

	if (sock && family == PF_UNIX) {
		isec = inode_security_novalidate(SOCK_INODE(sock));
		peer_secid = isec->sid;
	} else if (skb)
		selinux_skb_peerlbl_sid(skb, family, &peer_secid);

out:
	*secid = peer_secid;
	if (peer_secid == SECSID_NULL)
		return -EINVAL;
	return 0;
}