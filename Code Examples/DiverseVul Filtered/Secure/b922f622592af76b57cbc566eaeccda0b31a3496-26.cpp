int hw_atl_utils_fw_rpc_call(struct aq_hw_s *self, unsigned int rpc_size)
{
	struct aq_hw_atl_utils_fw_rpc_tid_s sw;
	int err = 0;

	if (!ATL_HW_IS_CHIP_FEATURE(self, MIPS)) {
		err = -1;
		goto err_exit;
	}
	err = hw_atl_write_fwcfg_dwords(self, (u32 *)(void *)&self->rpc,
					(rpc_size + sizeof(u32) -
					 sizeof(u8)) / sizeof(u32));
	if (err < 0)
		goto err_exit;

	sw.tid = 0xFFFFU & (++self->rpc_tid);
	sw.len = (u16)rpc_size;
	aq_hw_write_reg(self, HW_ATL_RPC_CONTROL_ADR, sw.val);

err_exit:
	return err;
}