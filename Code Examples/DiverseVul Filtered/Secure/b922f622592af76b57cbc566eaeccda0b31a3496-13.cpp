static u32 aq_fw1x_rpc_get(struct aq_hw_s *self)
{
	return aq_hw_read_reg(self, HW_ATL_MPI_RPC_ADDR);
}