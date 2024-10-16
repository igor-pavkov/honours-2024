static NTSTATUS get_lanman2_dir_entry(TALLOC_CTX *ctx,
				connection_struct *conn,
				struct dptr_struct *dirptr,
				uint16_t flags2,
				const char *path_mask,
				uint32_t dirtype,
				int info_level,
				bool requires_resume_key,
				bool dont_descend,
				bool ask_sharemode,
				char **ppdata,
				char *base_data,
				char *end_data,
				int space_remaining,
				bool *got_exact_match,
				int *last_entry_off,
				struct ea_list *name_list)
{
	uint8_t align = 4;
	const bool do_pad = true;

	if (info_level >= 1 && info_level <= 3) {
		/* No alignment on earlier info levels. */
		align = 1;
	}

	return smbd_dirptr_lanman2_entry(ctx, conn, dirptr, flags2,
					 path_mask, dirtype, info_level,
					 requires_resume_key, dont_descend, ask_sharemode,
					 true, align, do_pad,
					 ppdata, base_data, end_data,
					 space_remaining,
					 NULL,
					 got_exact_match,
					 last_entry_off, name_list, NULL);
}