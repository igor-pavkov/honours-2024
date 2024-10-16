static int tun_sendmsg(struct kiocb *iocb, struct socket *sock,
		       struct msghdr *m, size_t total_len)
{
	struct tun_struct *tun = container_of(sock, struct tun_struct, socket);
	return tun_get_user(tun, m->msg_control, m->msg_iov, total_len,
			    m->msg_iovlen, m->msg_flags & MSG_DONTWAIT);
}