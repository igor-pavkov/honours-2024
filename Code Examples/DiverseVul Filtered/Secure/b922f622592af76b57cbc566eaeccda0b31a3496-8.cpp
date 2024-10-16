int hw_atl_write_fwsettings_dwords(struct aq_hw_s *self, u32 offset, u32 *p,
				   u32 cnt)
{
	return hw_atl_utils_fw_upload_dwords(self, self->settings_addr + offset,
					     p, cnt, MCP_AREA_SETTINGS);
}