static void av1_decode_frame_wrapup(AV1State *state)
{
	u32 i;
	for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
		if ((state->frame_state.refresh_frame_flags >> i) & 1) {
			state->RefOrderHint[i] = state->frame_state.order_hint;
			state->SavedGmParams[i] = state->GmParams;
			state->RefFrameType[i] = state->frame_state.frame_type;
			state->RefUpscaledWidth[i] = state->UpscaledWidth;
			state->RefFrameHeight[i] = state->height;
		}
	}
	state->frame_state.seen_frame_header = GF_FALSE;
	//Otherwise (show_existing_frame is equal to 1), if frame_type is equal to KEY_FRAME, the reference frame loading process as specified in section 7.21 is invoked
	if ((state->frame_state.show_existing_frame) && (state->frame_state.frame_type == AV1_KEY_FRAME)) {
		state->frame_state.order_hint = state->RefOrderHint[state->frame_state.frame_to_show_map_idx];
		//OrderHints[ j + LAST_FRAME ] is set equal to SavedOrderHints[state->frame_to_show_map_idx ][ j + LAST_FRAME ] for j = 0..REFS_PER_FRAME-1.

		//gm_params[ ref ][ j ] is set equal to SavedGmParams[ frame_to_show_map_idx ][ ref ][ j ] for ref = LAST_FRAME..ALTREF_FRAME, for j = 0..5.
		state->GmParams = state->SavedGmParams[state->frame_state.frame_to_show_map_idx];

	}
}