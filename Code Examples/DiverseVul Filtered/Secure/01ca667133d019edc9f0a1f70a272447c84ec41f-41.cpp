static u8 fm10k_tx_desc_flags(struct sk_buff *skb, u32 tx_flags)
{
	/* set type for advanced descriptor with frame checksum insertion */
	u32 desc_flags = 0;

	/* set checksum offload bits */
	desc_flags |= FM10K_SET_FLAG(tx_flags, FM10K_TX_FLAGS_CSUM,
				     FM10K_TXD_FLAG_CSUM);

	return desc_flags;
}