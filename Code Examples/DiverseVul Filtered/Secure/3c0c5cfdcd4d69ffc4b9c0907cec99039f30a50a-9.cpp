static int pvc_connect(struct socket *sock, struct sockaddr *sockaddr,
		       int sockaddr_len, int flags)
{
	return pvc_bind(sock, sockaddr, sockaddr_len);
}