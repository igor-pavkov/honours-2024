static void frame_size_with_refs(GF_BitStream *bs, AV1State *state, Bool frame_size_override_flag, s8 *ref_frame_idx)
{
	Bool found_ref = GF_FALSE;
	u32 i = 0;
	for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
		found_ref = gf_bs_read_int_log_idx(bs, 1, "found_ref", i);
		if (found_ref == 1) {
			state->UpscaledWidth = state->RefUpscaledWidth[ref_frame_idx[i]];
			state->width = state->UpscaledWidth;
			state->height = state->RefFrameHeight[ref_frame_idx[i]];
			break;
		}
	}
	if (found_ref == 0) {
		av1_frame_size(bs, state, frame_size_override_flag);
		av1_render_size(bs);
	}
	else {
		superres_params(bs, state);
		//compute_image_size();
	}
}