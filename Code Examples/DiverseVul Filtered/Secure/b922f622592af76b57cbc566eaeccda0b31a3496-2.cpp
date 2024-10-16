static int hw_atl_utils_write_b0_mbox(struct aq_hw_s *self, u32 addr,
				      u32 *p, u32 cnt)
{
	u32 offset = 0;
	int err = 0;
	u32 val;

	aq_hw_write_reg(self, 0x208, addr);

	for (; offset < cnt; ++offset) {
		aq_hw_write_reg(self, 0x20C, p[offset]);
		aq_hw_write_reg(self, 0x200, 0xC000);

		err = readx_poll_timeout_atomic(hw_atl_utils_mif_cmd_get,
						self, val,
						(val & 0x100) == 0U,
						10U, 10000U);

		if (err < 0)
			break;
	}

	return err;
}