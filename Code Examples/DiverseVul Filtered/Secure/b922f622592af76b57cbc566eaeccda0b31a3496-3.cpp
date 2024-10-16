static int hw_atl_utils_mpi_create(struct aq_hw_s *self)
{
	int err = 0;

	err = hw_atl_utils_init_ucp(self, self->aq_nic_cfg->aq_hw_caps);
	if (err < 0)
		goto err_exit;

	err = hw_atl_utils_fw_rpc_init(self);
	if (err < 0)
		goto err_exit;

err_exit:
	return err;
}