static int hw_atl_utils_mpi_set_state(struct aq_hw_s *self,
				      enum hal_atl_utils_fw_state_e state)
{
	u32 val = aq_hw_read_reg(self, HW_ATL_MPI_CONTROL_ADR);
	struct hw_atl_utils_mbox_header mbox;
	u32 transaction_id = 0;
	int err = 0;

	if (state == MPI_RESET) {
		hw_atl_utils_mpi_read_mbox(self, &mbox);

		transaction_id = mbox.transaction_id;

		err = readx_poll_timeout_atomic(hw_atl_utils_get_mpi_mbox_tid,
						self, mbox.transaction_id,
						transaction_id !=
						mbox.transaction_id,
						1000U, 100000U);
		if (err < 0)
			goto err_exit;
	}
	/* On interface DEINIT we disable DW (raise bit)
	 * Otherwise enable DW (clear bit)
	 */
	if (state == MPI_DEINIT || state == MPI_POWER)
		val |= HW_ATL_MPI_DIRTY_WAKE_MSK;
	else
		val &= ~HW_ATL_MPI_DIRTY_WAKE_MSK;

	/* Set new state bits */
	val = val & ~HW_ATL_MPI_STATE_MSK;
	val |= state & HW_ATL_MPI_STATE_MSK;

	aq_hw_write_reg(self, HW_ATL_MPI_CONTROL_ADR, val);

err_exit:
	return err;
}