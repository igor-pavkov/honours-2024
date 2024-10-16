GF_Err dOps_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_OpusSpecificBox *ptr = (GF_OpusSpecificBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	//we always write 0 - the version may also be set to 1 when fed from ffenc or opus+ogg
	gf_bs_write_u8(bs, /*ptr->opcfg.version*/ 0);
	gf_bs_write_u8(bs, ptr->opcfg.OutputChannelCount);
	gf_bs_write_u16(bs, ptr->opcfg.PreSkip);
	gf_bs_write_u32(bs, ptr->opcfg.InputSampleRate);
	gf_bs_write_u16(bs, ptr->opcfg.OutputGain);
	gf_bs_write_u8(bs, ptr->opcfg.ChannelMappingFamily);
	if (ptr->opcfg.ChannelMappingFamily) {
		gf_bs_write_u8(bs, ptr->opcfg.StreamCount);
		gf_bs_write_u8(bs, ptr->opcfg.CoupledCount);
		gf_bs_write_data(bs, (char *) ptr->opcfg.ChannelMapping, ptr->opcfg.OutputChannelCount);
	}
	return GF_OK;
}