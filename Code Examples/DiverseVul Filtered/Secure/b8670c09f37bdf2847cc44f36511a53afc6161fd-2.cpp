static inline __be16 llc_proto_type(u16 arphrd)
{
	return htons(ETH_P_802_2);
}