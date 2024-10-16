int hw_atl_utils_soft_reset(struct aq_hw_s *self)
{
	int ver = hw_atl_utils_get_fw_version(self);
	u32 boot_exit_code = 0;
	u32 val;
	int k;

	for (k = 0; k < 1000; ++k) {
		u32 flb_status = aq_hw_read_reg(self,
						HW_ATL_MPI_DAISY_CHAIN_STATUS);
		boot_exit_code = aq_hw_read_reg(self,
						HW_ATL_MPI_BOOT_EXIT_CODE);
		if (flb_status != 0x06000000 || boot_exit_code != 0)
			break;
	}

	if (k == 1000) {
		aq_pr_err("Neither RBL nor FLB firmware started\n");
		return -EOPNOTSUPP;
	}

	self->rbl_enabled = (boot_exit_code != 0);

	if (hw_atl_utils_ver_match(HW_ATL_FW_VER_1X, ver)) {
		int err = 0;

		/* FW 1.x may bootup in an invalid POWER state (WOL feature).
		 * We should work around this by forcing its state back to DEINIT
		 */
		hw_atl_utils_mpi_set_state(self, MPI_DEINIT);
		err = readx_poll_timeout_atomic(hw_atl_utils_mpi_get_state,
						self, val,
						(val & HW_ATL_MPI_STATE_MSK) ==
						 MPI_DEINIT,
						10, 10000U);
		if (err)
			return err;
	} else if (hw_atl_utils_ver_match(HW_ATL_FW_VER_4X, ver)) {
		u64 sem_timeout = aq_hw_read_reg(self, HW_ATL_MIF_RESET_TIMEOUT_ADR);

		/* Acquire 2 semaphores before issuing reset for FW 4.x */
		if (sem_timeout > 3000)
			sem_timeout = 3000;
		sem_timeout = sem_timeout * 1000;

		if (sem_timeout != 0) {
			int err;

			err = readx_poll_timeout_atomic(hw_atl_sem_reset1_get, self, val,
							val == 1U, 1U, sem_timeout);
			if (err)
				aq_pr_err("reset sema1 timeout");

			err = readx_poll_timeout_atomic(hw_atl_sem_reset2_get, self, val,
							val == 1U, 1U, sem_timeout);
			if (err)
				aq_pr_err("reset sema2 timeout");
		}
	}

	if (self->rbl_enabled)
		return hw_atl_utils_soft_reset_rbl(self);
	else
		return hw_atl_utils_soft_reset_flb(self);
}