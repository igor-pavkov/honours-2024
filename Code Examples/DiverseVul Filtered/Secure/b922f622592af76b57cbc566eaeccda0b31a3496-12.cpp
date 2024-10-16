u32 hw_atl_utils_get_fw_version(struct aq_hw_s *self)
{
	return aq_hw_read_reg(self, HW_ATL_MPI_FW_VERSION);
}