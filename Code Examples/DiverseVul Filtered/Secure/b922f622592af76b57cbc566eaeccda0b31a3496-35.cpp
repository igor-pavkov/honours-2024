static u32 hw_atl_utils_mif_cmd_get(struct aq_hw_s *self)
{
	return aq_hw_read_reg(self, HW_ATL_MIF_CMD);
}