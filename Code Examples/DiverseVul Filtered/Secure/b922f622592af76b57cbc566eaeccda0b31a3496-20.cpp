static int hw_atl_utils_mpi_set_speed(struct aq_hw_s *self, u32 speed)
{
	u32 val = aq_hw_read_reg(self, HW_ATL_MPI_CONTROL_ADR);

	val = val & ~HW_ATL_MPI_SPEED_MSK;
	val |= speed << HW_ATL_MPI_SPEED_SHIFT;
	aq_hw_write_reg(self, HW_ATL_MPI_CONTROL_ADR, val);

	return 0;
}