static Bool vvc_parse_nal_header(GF_BitStream *bs, u8 *nal_unit_type, u8 *temporal_id, u8 *layer_id)
{
	u32 val;
	val = gf_bs_read_int_log(bs, 1, "forbidden_zero");
	if (val) return GF_FALSE;
	val = gf_bs_read_int_log(bs, 1, "reserved_zero");
	if (val) return GF_FALSE;

	val = gf_bs_read_int_log(bs, 6, "layerID");
	if (layer_id) *layer_id = val;

	val = gf_bs_read_int_log(bs, 5, "nuh_type");
	if (nal_unit_type) *nal_unit_type = val;

	val = gf_bs_read_int_log(bs, 3, "temporalID");
	if (!val) return GF_FALSE;
	val -= 1;
	if (temporal_id) *temporal_id = val;
	return GF_TRUE;
}