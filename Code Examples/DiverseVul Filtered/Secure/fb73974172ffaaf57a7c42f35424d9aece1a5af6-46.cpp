static int selinux_socket_getsockname(struct socket *sock)
{
	return sock_has_perm(sock->sk, SOCKET__GETATTR);
}