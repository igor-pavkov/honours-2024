s32 gf_hevc_parse_nalu(u8 *data, u32 size, HEVCState *hevc, u8 *nal_unit_type, u8 *temporal_id, u8 *layer_id)
{
	GF_BitStream *bs = NULL;
	s32 ret = -1;

	if (size<2) return -1;

	if (!hevc) {
		if (nal_unit_type) (*nal_unit_type) = (data[0] & 0x7E) >> 1;
		if (layer_id) {
			u8 id = data[0] & 1;
			id <<= 5;
			id |= (data[1] >> 3) & 0x1F;
			(*layer_id) = id;
		}
		if (temporal_id) (*temporal_id) = (data[1] & 0x7);
		return -1;
	}

	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	if (!bs) return -1;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	ret = gf_hevc_parse_nalu_bs(bs, hevc, nal_unit_type, temporal_id, layer_id);

	gf_bs_del(bs);
	return ret;
}