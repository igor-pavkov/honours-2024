static int hw_atl_fw1x_deinit(struct aq_hw_s *self)
{
	hw_atl_utils_mpi_set_speed(self, 0);
	hw_atl_utils_mpi_set_state(self, MPI_DEINIT);

	return 0;
}