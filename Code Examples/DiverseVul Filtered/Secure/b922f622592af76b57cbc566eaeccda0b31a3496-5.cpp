int hw_atl_utils_mpi_read_mbox(struct aq_hw_s *self,
			       struct hw_atl_utils_mbox_header *pmbox)
{
	return hw_atl_utils_fw_downld_dwords(self,
					     self->mbox_addr,
					     (u32 *)(void *)pmbox,
					     sizeof(*pmbox) / sizeof(u32));
}