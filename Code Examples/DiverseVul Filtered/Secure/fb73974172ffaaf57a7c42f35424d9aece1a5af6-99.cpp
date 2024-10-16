static int selinux_socket_shutdown(struct socket *sock, int how)
{
	return sock_has_perm(sock->sk, SOCKET__SHUTDOWN);
}