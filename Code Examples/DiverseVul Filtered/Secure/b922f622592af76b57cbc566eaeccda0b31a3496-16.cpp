static u32 hw_atl_utils_rpc_state_get(struct aq_hw_s *self)
{
	return aq_hw_read_reg(self, HW_ATL_RPC_STATE_ADR);
}