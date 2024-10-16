static int selinux_socket_sendmsg(struct socket *sock, struct msghdr *msg,
				  int size)
{
	return sock_has_perm(sock->sk, SOCKET__WRITE);
}