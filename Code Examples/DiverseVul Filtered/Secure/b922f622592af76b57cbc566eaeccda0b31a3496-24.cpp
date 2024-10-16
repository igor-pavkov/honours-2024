static int hw_atl_utils_fw_upload_dwords(struct aq_hw_s *self, u32 addr, u32 *p,
					 u32 cnt, enum mcp_area area)
{
	int err = 0;
	u32 val;

	err = readx_poll_timeout_atomic(hw_atl_sem_ram_get, self,
					val, val == 1U,
					10U, 100000U);
	if (err < 0)
		goto err_exit;

	if (ATL_HW_IS_CHIP_FEATURE(self, REVISION_B1))
		err = hw_atl_utils_write_b1_mbox(self, addr, p, cnt, area);
	else
		err = hw_atl_utils_write_b0_mbox(self, addr, p, cnt);

	hw_atl_reg_glb_cpu_sem_set(self, 1U, HW_ATL_FW_SM_RAM);

	if (err < 0)
		goto err_exit;

	err = aq_hw_err_from_flags(self);

err_exit:
	return err;
}