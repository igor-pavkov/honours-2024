static bool notify_marshall_changes(int num_changes,
				uint32_t max_offset,
				struct notify_change_event *changes,
				DATA_BLOB *final_blob)
{
	int i;

	if (num_changes == -1) {
		return false;
	}

	/*
	 * Sort the notifies by timestamp when the event happened to avoid
	 * coalescing and thus dropping events.
	 */

	qsort(changes, num_changes,
	      sizeof(*changes), compare_notify_change_events);

	for (i=0; i<num_changes; i++) {
		enum ndr_err_code ndr_err;
		struct notify_change_event *c;
		struct FILE_NOTIFY_INFORMATION m;
		DATA_BLOB blob;
		uint16_t pad = 0;

		/* Coalesce any identical records. */
		while (i+1 < num_changes &&
			notify_change_record_identical(&changes[i],
						&changes[i+1])) {
			i++;
		}

		c = &changes[i];

		m.FileName1 = c->name;
		m.FileNameLength = strlen_m(c->name)*2;
		m.Action = c->action;

		m._pad = data_blob_null;

		/*
		 * Offset to next entry, only if there is one
		 */

		if (i == (num_changes-1)) {
			m.NextEntryOffset = 0;
		} else {
			if ((m.FileNameLength % 4) == 2) {
				m._pad = data_blob_const(&pad, 2);
			}
			m.NextEntryOffset =
				ndr_size_FILE_NOTIFY_INFORMATION(&m, 0);
		}

		ndr_err = ndr_push_struct_blob(&blob, talloc_tos(), &m,
			(ndr_push_flags_fn_t)ndr_push_FILE_NOTIFY_INFORMATION);
		if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
			return false;
		}

		if (DEBUGLEVEL >= 10) {
			NDR_PRINT_DEBUG(FILE_NOTIFY_INFORMATION, &m);
		}

		if (!data_blob_append(talloc_tos(), final_blob,
				      blob.data, blob.length)) {
			data_blob_free(&blob);
			return false;
		}

		data_blob_free(&blob);

		if (final_blob->length > max_offset) {
			/* Too much data for client. */
			DEBUG(10, ("Client only wanted %d bytes, trying to "
				   "marshall %d bytes\n", (int)max_offset,
				   (int)final_blob->length));
			return False;
		}
	}

	return True;
}