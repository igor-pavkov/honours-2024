static int selinux_socket_listen(struct socket *sock, int backlog)
{
	return sock_has_perm(sock->sk, SOCKET__LISTEN);
}