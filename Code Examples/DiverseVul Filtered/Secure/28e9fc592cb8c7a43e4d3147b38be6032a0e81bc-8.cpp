static inline __be16 llc_proto_type(u16 arphrd)
{
	return arphrd == ARPHRD_IEEE802_TR ?
			 htons(ETH_P_TR_802_2) : htons(ETH_P_802_2);
}