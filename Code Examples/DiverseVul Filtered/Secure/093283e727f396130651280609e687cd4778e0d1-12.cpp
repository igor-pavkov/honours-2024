GF_Err abst_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	unsigned int i;
	GF_AdobeBootstrapInfoBox *ptr = (GF_AdobeBootstrapInfoBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->bootstrapinfo_version);
	gf_bs_write_int(bs, ptr->profile, 2);
	gf_bs_write_int(bs, ptr->live, 1);
	gf_bs_write_int(bs, ptr->update, 1);
	gf_bs_write_int(bs, ptr->reserved, 4);
	gf_bs_write_u32(bs, ptr->time_scale);
	gf_bs_write_u64(bs, ptr->current_media_time);
	gf_bs_write_u64(bs, ptr->smpte_time_code_offset);
	if (ptr->movie_identifier)
		gf_bs_write_data(bs, ptr->movie_identifier, (u32)strlen(ptr->movie_identifier) + 1);
	else
		gf_bs_write_u8(bs, 0);

	gf_bs_write_u8(bs, ptr->server_entry_count);
	for (i=0; i<ptr->server_entry_count; i++) {
		char *str = (char*)gf_list_get(ptr->server_entry_table, i);
		gf_bs_write_data(bs, str, (u32)strlen(str) + 1);
	}

	gf_bs_write_u8(bs, ptr->quality_entry_count);
	for (i=0; i<ptr->quality_entry_count; i++) {
		char *str = (char*)gf_list_get(ptr->quality_entry_table, i);
		gf_bs_write_data(bs, str, (u32)strlen(str) + 1);
	}

	if (ptr->drm_data)
		gf_bs_write_data(bs, ptr->drm_data, (u32)strlen(ptr->drm_data) + 1);
	else
		gf_bs_write_u8(bs, 0);

	if (ptr->meta_data)
		gf_bs_write_data(bs, ptr->meta_data, (u32)strlen(ptr->meta_data) + 1);
	else
		gf_bs_write_u8(bs, 0);

	gf_bs_write_u8(bs, ptr->segment_run_table_count);
	for (i=0; i<ptr->segment_run_table_count; i++) {
		e = gf_isom_box_write((GF_Box *)gf_list_get(ptr->segment_run_table_entries, i), bs);
		if (e) return e;
	}

	gf_bs_write_u8(bs, ptr->fragment_run_table_count);
	for (i=0; i<ptr->fragment_run_table_count; i++) {
		e = gf_isom_box_write((GF_Box *)gf_list_get(ptr->fragment_run_table_entries, i), bs);
		if (e) return e;
	}

	return GF_OK;
}