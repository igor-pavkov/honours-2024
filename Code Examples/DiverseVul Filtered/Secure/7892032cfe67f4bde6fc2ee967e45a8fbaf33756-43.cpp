static int gre_handle_offloads(struct sk_buff *skb, bool csum)
{
	return iptunnel_handle_offloads(skb,
					csum ? SKB_GSO_GRE_CSUM : SKB_GSO_GRE);
}