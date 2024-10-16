void update_free_window_state(WINDOW_STATE_ORDER* window_state)
{
	if (!window_state)
		return;

	free(window_state->OverlayDescription.string);
	free(window_state->titleInfo.string);
	free(window_state->windowRects);
	free(window_state->visibilityRects);
	memset(window_state, 0, sizeof(WINDOW_STATE_ORDER));
}