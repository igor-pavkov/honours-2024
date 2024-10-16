GF_Err aom_av1_parse_temporal_unit_from_annexb(GF_BitStream *bs, AV1State *state)
{
	GF_Err e;
	u64 tupos;
	u64 tusize, sz;
	if (!bs || !state) return GF_BAD_PARAM;

	gf_bs_mark_overflow(bs, GF_TRUE);
	tusize = sz = gf_av1_leb128_read(bs, NULL);
	tupos = gf_bs_get_position(bs);
	if (!sz) {
		GF_LOG(GF_LOG_INFO, GF_LOG_CODING, ("[AV1] temporal unit size is 0, likely not annex B\n"));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1] Annex B temporal unit detected (size "LLU") ***** \n", sz));
	while (sz > 0) {
		u8 Leb128Bytes = 0;
		u64 frame_unit_size = gf_av1_leb128_read(bs, &Leb128Bytes);

		if (gf_bs_is_overflow(bs)) {
			return GF_BUFFER_TOO_SMALL;
		}
		if (sz < Leb128Bytes + frame_unit_size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] Annex B sz("LLU") < Leb128Bytes("LLU") + frame_unit_size("LLU")\n", sz, Leb128Bytes, frame_unit_size));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1] Annex B frame unit detected (size "LLU")\n", frame_unit_size));
		sz -= Leb128Bytes + frame_unit_size;

		while (frame_unit_size > 0) {
			u64 pos, obu_size = gf_av1_leb128_read(bs, &Leb128Bytes);

			if (gf_bs_is_overflow(bs)) {
				return GF_BUFFER_TOO_SMALL;
			}
			if (frame_unit_size < Leb128Bytes + obu_size) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] Annex B frame_unit_size("LLU") < Leb128Bytes("LLU") + obu_length("LLU")\n", frame_unit_size, Leb128Bytes, obu_size));
				return GF_NON_COMPLIANT_BITSTREAM;
			}
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1] Annex B OBU detected (size "LLU")\n", obu_size));
			pos = gf_bs_get_position(bs);
			frame_unit_size -= Leb128Bytes;

			e = gf_av1_parse_obu(bs, &state->obu_type, &obu_size, NULL, state);
			if (e) return e;

			if (obu_size != gf_bs_get_position(bs) - pos) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[AV1] Annex B frame size "LLU" different from consumed bytes "LLU".\n", obu_size, gf_bs_get_position(bs) - pos));
				return GF_NON_COMPLIANT_BITSTREAM;
			}

			av1_populate_state_from_obu(bs, pos, obu_size, state->obu_type, state);
			if (frame_unit_size < obu_size) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] Annex B frame_unit_size("LLU") < OBU size ("LLU")\n", frame_unit_size, obu_size));
				return GF_NON_COMPLIANT_BITSTREAM;
			}
			frame_unit_size -= obu_size;
		}
	}
	assert(sz == 0);
	if (tusize != gf_bs_get_position(bs) - tupos) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[AV1] Annex B TU size "LLU" different from consumed bytes "LLU".\n", tusize, gf_bs_get_position(bs) - tupos));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	return GF_OK;
}