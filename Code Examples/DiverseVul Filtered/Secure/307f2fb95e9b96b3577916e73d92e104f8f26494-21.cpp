static inline bool rt6_qualify_for_ecmp(struct rt6_info *rt)
{
	return (rt->rt6i_flags & (RTF_GATEWAY|RTF_ADDRCONF|RTF_DYNAMIC)) ==
	       RTF_GATEWAY;
}