GF_Err emsg_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_Err e;
	GF_EventMessageBox *ptr = (GF_EventMessageBox*) s;

	if (ptr->version==0) {
		e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->scheme_id_uri);
		if (e) return e;
		e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->value);
		if (e) return e;

		ISOM_DECREASE_SIZE(ptr, 16);
		ptr->timescale = gf_bs_read_u32(bs);
		ptr->presentation_time_delta = gf_bs_read_u32(bs);
		ptr->event_duration = gf_bs_read_u32(bs);
		ptr->event_id = gf_bs_read_u32(bs);
	} else if (ptr->version==1) {
		ISOM_DECREASE_SIZE(ptr, 20);
		ptr->timescale = gf_bs_read_u32(bs);
		ptr->presentation_time_delta = gf_bs_read_u64(bs);
		ptr->event_duration = gf_bs_read_u32(bs);
		ptr->event_id = gf_bs_read_u32(bs);

		e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->scheme_id_uri);
		if (e) return e;
		e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->value);
		if (e) return e;
	} else {
		return GF_OK;
	}
	if (ptr->size) {
		if (ptr->size>0xFFFFFFFUL) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[IsoMedia] emsg message data size too big ("LLU") to be loaded\n", ptr->size));
			return GF_OUT_OF_MEM;
		}
		ptr->message_data_size = (u32) ptr->size;
		ptr->message_data = gf_malloc(ptr->message_data_size);
		if (!ptr->message_data) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->message_data, ptr->message_data_size);
		ptr->size = 0;
	}
	return GF_OK;
}