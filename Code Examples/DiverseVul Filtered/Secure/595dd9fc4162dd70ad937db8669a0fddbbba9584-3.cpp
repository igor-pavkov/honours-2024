NTSTATUS winbindd_lookupsids_recv(struct tevent_req *req,
				  struct winbindd_response *response)
{
	struct winbindd_lookupsids_state *state = tevent_req_data(
		req, struct winbindd_lookupsids_state);
	NTSTATUS status;
	char *result;
	uint32_t i;

	if (tevent_req_is_nterror(req, &status)) {
		DEBUG(5, ("wb_lookupsids failed: %s\n", nt_errstr(status)));
		return status;
	}

	result = talloc_asprintf(response, "%d\n", (int)state->domains->count);
	if (result == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	for (i=0; i<state->domains->count; i++) {
		struct dom_sid_buf sid_str;

		result = talloc_asprintf_append_buffer(
			result, "%s %s\n",
			dom_sid_str_buf(state->domains->domains[i].sid,
					&sid_str),
			state->domains->domains[i].name.string);
		if (result == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
	}

	result = talloc_asprintf_append_buffer(
		result, "%d\n", (int)state->names->count);
	if (result == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	for (i=0; i<state->names->count; i++) {
		struct lsa_TranslatedName *name;

		name = &state->names->names[i];

		result = talloc_asprintf_append_buffer(
			result, "%d %d %s\n",
			(int)name->sid_index, (int)name->sid_type,
			name->name.string);
		if (result == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
	}

	response->extra_data.data = result;
	response->length += talloc_get_size(result);
	return NT_STATUS_OK;
}