static void av1_parse_obu_metadata(AV1State *state, GF_BitStream *bs)
{
	u32 metadata_type = (u32)gf_av1_leb128_read(bs, NULL);

	switch (metadata_type) {
	case OBU_METADATA_TYPE_ITUT_T35:
		break;
	case OBU_METADATA_TYPE_HDR_CLL:
		gf_bs_read_data(bs, state->clli_data, 4);
		state->clli_valid = 1;
		break;
	case OBU_METADATA_TYPE_HDR_MDCV:
		gf_bs_read_data(bs, state->mdcv_data, 24);
		state->mdcv_valid = 1;
		break;
	default:
		break;
	}
}