static const char* inet_n46top(const struct in6_addr* in,
			       char* buf, size_t buf_len)
{
	if (IN6_IS_ADDR_V4MAPPED(in)) {
		return inet_ntop(AF_INET, ((uint32_t*)(in->s6_addr))+3, buf, buf_len);
	} else {
		return inet_ntop(AF_INET6, in, buf, buf_len);
	}
}