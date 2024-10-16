static u32 hw_atl_utils_mpi_get_state(struct aq_hw_s *self)
{
	return aq_hw_read_reg(self, HW_ATL_MPI_STATE_ADR);
}