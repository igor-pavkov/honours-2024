GF_Err emsg_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 len;
	GF_EventMessageBox *ptr = (GF_EventMessageBox*) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	if (ptr->version==1) {
		gf_bs_write_u32(bs, ptr->timescale);
		gf_bs_write_u64(bs, ptr->presentation_time_delta);
		gf_bs_write_u32(bs, ptr->event_duration);
		gf_bs_write_u32(bs, ptr->event_id);
	}

	len = ptr->scheme_id_uri ? (u32) strlen(ptr->scheme_id_uri) : 0;
	if (len) gf_bs_write_data(bs, ptr->scheme_id_uri, len);
	gf_bs_write_u8(bs, 0);

	len = ptr->value ? (u32) strlen(ptr->value) : 0;
	if (len) gf_bs_write_data(bs, ptr->value, len);
	gf_bs_write_u8(bs, 0);

	if (ptr->version==0) {
		gf_bs_write_u32(bs, ptr->timescale);
		gf_bs_write_u32(bs, (u32) ptr->presentation_time_delta);
		gf_bs_write_u32(bs, ptr->event_duration);
		gf_bs_write_u32(bs, ptr->event_id);
	}
	if (ptr->message_data)
		gf_bs_write_data(bs, ptr->message_data, ptr->message_data_size);
	return GF_OK;
}