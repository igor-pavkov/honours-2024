int hw_atl_write_fwcfg_dwords(struct aq_hw_s *self, u32 *p, u32 cnt)
{
	return hw_atl_utils_fw_upload_dwords(self, self->rpc_addr, p,
					     cnt, MCP_AREA_CONFIG);
}