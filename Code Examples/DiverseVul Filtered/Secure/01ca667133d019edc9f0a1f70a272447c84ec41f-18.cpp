static struct ethhdr *fm10k_gre_is_nvgre(struct sk_buff *skb)
{
	struct fm10k_nvgre_hdr *nvgre_hdr;
	int hlen = ip_hdrlen(skb);

	/* currently only IPv4 is supported due to hlen above */
	if (vlan_get_protocol(skb) != htons(ETH_P_IP))
		return NULL;

	/* our transport header should be NVGRE */
	nvgre_hdr = (struct fm10k_nvgre_hdr *)(skb_network_header(skb) + hlen);

	/* verify all reserved flags are 0 */
	if (nvgre_hdr->flags & FM10K_NVGRE_RESERVED0_FLAGS)
		return NULL;

	/* report start of ethernet header */
	if (nvgre_hdr->flags & NVGRE_TNI)
		return (struct ethhdr *)(nvgre_hdr + 1);

	return (struct ethhdr *)(&nvgre_hdr->tni);
}