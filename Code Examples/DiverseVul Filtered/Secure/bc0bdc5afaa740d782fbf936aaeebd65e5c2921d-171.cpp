static inline u8 cma_user_data_offset(struct rdma_id_private *id_priv)
{
	return cma_family(id_priv) == AF_IB ? 0 : sizeof(struct cma_hdr);
}