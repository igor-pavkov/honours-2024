GF_Err aom_av1_parse_temporal_unit_from_section5(GF_BitStream *bs, AV1State *state)
{
	if (!state) return GF_BAD_PARAM;
	state->obu_type = -1;

	while (state->obu_type != OBU_TEMPORAL_DELIMITER) {
		GF_Err e;
		if (!gf_bs_available(bs))
			return state->unframed ? GF_BUFFER_TOO_SMALL : GF_OK;

		u64 pos = gf_bs_get_position(bs), obu_size = 0;

		e = gf_av1_parse_obu(bs, &state->obu_type, &obu_size, NULL, state);
		if (e)
			return e;

		if (obu_size != gf_bs_get_position(bs) - pos) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[AV1] OBU (Section 5) frame size "LLU" different from consumed bytes "LLU".\n", obu_size, gf_bs_get_position(bs) - pos));
			return GF_NON_COMPLIANT_BITSTREAM;
		}

		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1] Section5 OBU detected (size "LLU")\n", obu_size));
		av1_populate_state_from_obu(bs, pos, obu_size, state->obu_type, state);
	}

	return GF_OK;
}