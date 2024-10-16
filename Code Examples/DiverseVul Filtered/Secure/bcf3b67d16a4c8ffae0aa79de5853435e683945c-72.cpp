void megasas_set_dma_settings(struct megasas_instance *instance,
			      struct megasas_dcmd_frame *dcmd,
			      dma_addr_t dma_addr, u32 dma_len)
{
	if (instance->consistent_mask_64bit) {
		dcmd->sgl.sge64[0].phys_addr = cpu_to_le64(dma_addr);
		dcmd->sgl.sge64[0].length = cpu_to_le32(dma_len);
		dcmd->flags = cpu_to_le16(dcmd->flags | MFI_FRAME_SGL64);

	} else {
		dcmd->sgl.sge32[0].phys_addr =
				cpu_to_le32(lower_32_bits(dma_addr));
		dcmd->sgl.sge32[0].length = cpu_to_le32(dma_len);
		dcmd->flags = cpu_to_le16(dcmd->flags);
	}
}