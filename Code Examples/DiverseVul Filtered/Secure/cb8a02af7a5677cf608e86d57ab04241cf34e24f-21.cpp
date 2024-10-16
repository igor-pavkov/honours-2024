static void FillSA(struct sockaddr *sa, const struct in6_addr *in6,
		uint16_t port)
{
	if (IN6_IS_ADDR_V4MAPPED(in6)) {
		struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
		sa4->sin_family = AF_INET;
		sa4->sin_addr.s_addr = ((uint32_t*)(in6)->s6_addr)[3];
		sa4->sin_port = htons(port);
	} else {
		struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)sa;
		sa6->sin6_family = AF_INET6;
		sa6->sin6_addr = *in6;
		sa6->sin6_port = htons(port);
	}
}