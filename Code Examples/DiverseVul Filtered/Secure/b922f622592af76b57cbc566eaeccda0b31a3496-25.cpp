static int aq_fw1x_set_power(struct aq_hw_s *self, unsigned int power_state,
			     const u8 *mac)
{
	struct hw_atl_utils_fw_rpc *prpc = NULL;
	unsigned int rpc_size = 0U;
	int err = 0;

	if (self->aq_nic_cfg->wol & WAKE_MAGIC) {
		err = aq_fw1x_set_wake_magic(self, 1, mac);

		if (err < 0)
			goto err_exit;

		rpc_size = sizeof(prpc->msg_id) +
			   sizeof(prpc->msg_enable_wakeup);

		err = hw_atl_utils_fw_rpc_wait(self, &prpc);

		if (err < 0)
			goto err_exit;

		memset(prpc, 0, rpc_size);

		prpc->msg_id = HAL_ATLANTIC_UTILS_FW_MSG_ENABLE_WAKEUP;
		prpc->msg_enable_wakeup.pattern_mask = 0x00000002;

		err = hw_atl_utils_fw_rpc_call(self, rpc_size);
		if (err < 0)
			goto err_exit;
	}
	hw_atl_utils_mpi_set_speed(self, 0);
	hw_atl_utils_mpi_set_state(self, MPI_POWER);

err_exit:
	return err;
}