s32 gf_vvc_parse_nalu(u8 *data, u32 size, VVCState *vvc, u8 *nal_unit_type, u8 *temporal_id, u8 *layer_id)
{
	GF_BitStream *bs = NULL;
	s32 ret;

	if (size<2) return -1;

	if (!vvc) {
		if (nal_unit_type) (*nal_unit_type) = data[1] >> 3;
		if (layer_id) (*layer_id) = data[0] & 0x3f;
		if (temporal_id) (*temporal_id) = (data[1] & 0x7);
		return -1;
	}
	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	if (!bs) return -1;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	ret = gf_vvc_parse_nalu_bs(bs, vvc, nal_unit_type, temporal_id, layer_id);
	gf_bs_del(bs);
	return ret;
}