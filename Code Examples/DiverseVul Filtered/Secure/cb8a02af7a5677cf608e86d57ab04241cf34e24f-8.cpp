static const char* inet_satop(struct sockaddr* sa, char* buf, size_t buf_len)
{
	if (sa->sa_family == AF_INET) {
		return inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), buf, buf_len);
	} else {
		return inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), buf, buf_len);
	}
}