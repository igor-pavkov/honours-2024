static int hw_atl_utils_write_b1_mbox(struct aq_hw_s *self, u32 addr,
				      u32 *p, u32 cnt, enum mcp_area area)
{
	u32 data_offset = 0;
	u32 offset = addr;
	int err = 0;
	u32 val;

	switch (area) {
	case MCP_AREA_CONFIG:
		offset -= self->rpc_addr;
		break;

	case MCP_AREA_SETTINGS:
		offset -= self->settings_addr;
		break;
	}

	offset = offset / sizeof(u32);

	for (; data_offset < cnt; ++data_offset, ++offset) {
		aq_hw_write_reg(self, 0x328, p[data_offset]);
		aq_hw_write_reg(self, 0x32C,
				(area | (0xFFFF & (offset * 4))));
		hw_atl_mcp_up_force_intr_set(self, 1);
		/* 1000 times by 10us = 10ms */
		err = readx_poll_timeout_atomic(hw_atl_scrpad12_get,
						self, val,
						(val & 0xF0000000) !=
						area,
						10U, 10000U);

		if (err < 0)
			break;
	}

	return err;
}