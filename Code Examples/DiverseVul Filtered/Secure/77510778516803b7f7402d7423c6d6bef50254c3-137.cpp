GF_Err dOps_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_OpusSpecificBox *ptr = (GF_OpusSpecificBox *)s;
	ptr->opcfg.version = gf_bs_read_u8(bs);
	if (ptr->opcfg.version) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[IsoMedia] Opus config version not 0 !\n", ptr->size));
		return GF_NOT_SUPPORTED;
	}
	//force version to 1 as used in ogg/opus
	ptr->opcfg.version = 1;
	ptr->opcfg.OutputChannelCount = gf_bs_read_u8(bs);
	ptr->opcfg.PreSkip = gf_bs_read_u16(bs);
	ptr->opcfg.InputSampleRate = gf_bs_read_u32(bs);
	ptr->opcfg.OutputGain = gf_bs_read_u16(bs);
	ptr->opcfg.ChannelMappingFamily = gf_bs_read_u8(bs);
	ISOM_DECREASE_SIZE(ptr, 11)
	if (ptr->size) {
		ISOM_DECREASE_SIZE(ptr, 2+ptr->opcfg.OutputChannelCount);
		ptr->opcfg.StreamCount = gf_bs_read_u8(bs);
		ptr->opcfg.CoupledCount = gf_bs_read_u8(bs);
		gf_bs_read_data(bs, (char *) ptr->opcfg.ChannelMapping, ptr->opcfg.OutputChannelCount);
	}
	return GF_OK;
}