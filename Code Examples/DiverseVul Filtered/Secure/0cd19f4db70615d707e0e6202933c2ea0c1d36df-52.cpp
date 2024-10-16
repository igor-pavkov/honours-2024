void gf_av1_reset_state(AV1State *state, Bool is_destroy)
{
	GF_List *l1, *l2;

	if (state->frame_state.header_obus) {
		while (gf_list_count(state->frame_state.header_obus)) {
			GF_AV1_OBUArrayEntry *a = (GF_AV1_OBUArrayEntry*)gf_list_pop_back(state->frame_state.header_obus);
			if (a->obu) gf_free(a->obu);
			gf_free(a);
		}
	}

	if (state->frame_state.frame_obus) {
		while (gf_list_count(state->frame_state.frame_obus)) {
			GF_AV1_OBUArrayEntry *a = (GF_AV1_OBUArrayEntry*)gf_list_pop_back(state->frame_state.frame_obus);
			if (a->obu) gf_free(a->obu);
			gf_free(a);
		}
	}
	l1 = state->frame_state.frame_obus;
	l2 = state->frame_state.header_obus;
	memset(&state->frame_state, 0, sizeof(AV1StateFrame));
	state->frame_state.is_first_frame = GF_TRUE;

	if (is_destroy) {
		gf_list_del(l1);
		gf_list_del(l2);
		if (state->bs) {
			u32 size, asize=0;
			u8 *ptr=NULL;
			//detach BS internal buffer
			gf_bs_get_content_no_truncate(state->bs, &ptr, &size, &asize);
			//avoid double free, cf issue 1893
			if (ptr != state->frame_obus) {
				gf_free(ptr);
			}
			if (state->frame_obus) {
				gf_free(state->frame_obus);
				state->frame_obus = NULL;
				state->frame_obus_alloc = 0;
			}
			gf_bs_del(state->bs);
		}
		state->bs = NULL;
	}
	else {
		state->frame_state.frame_obus = l1;
		state->frame_state.header_obus = l2;
		if (state->bs)
			gf_bs_seek(state->bs, 0);
	}
}