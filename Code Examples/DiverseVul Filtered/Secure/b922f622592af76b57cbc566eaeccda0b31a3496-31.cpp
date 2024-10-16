static u32 hw_atl_utils_get_mpi_mbox_tid(struct aq_hw_s *self)
{
	struct hw_atl_utils_mbox_header mbox;

	hw_atl_utils_mpi_read_mbox(self, &mbox);

	return mbox.transaction_id;
}