static int selinux_socket_getpeername(struct socket *sock)
{
	return sock_has_perm(sock->sk, SOCKET__GETATTR);
}