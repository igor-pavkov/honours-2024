static URI_INLINE UriBool URI_FUNC(OnExitOwnPortUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	state->uri->hostText.first = state->uri->userInfo.first; /* Host instead of userInfo, update */
	state->uri->userInfo.first = NULL; /* Not a userInfo, reset */
	state->uri->portText.afterLast = first; /* PORT END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = memory->malloc(memory, 1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (state->uri->hostData.ip4 == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		memory->free(memory, state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
	return URI_TRUE; /* Success */
}