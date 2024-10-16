static inline const char* proto_name(u16 pid)
{
	switch (pid) {
	case PID_LCP:
		return "LCP";
	case PID_IPCP:
		return "IPCP";
	case PID_IPV6CP:
		return "IPV6CP";
	default:
		return NULL;
	}
}