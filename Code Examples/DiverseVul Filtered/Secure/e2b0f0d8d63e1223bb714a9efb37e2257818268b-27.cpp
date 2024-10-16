static struct avrcp_data *data_init(struct avrcp *session, const char *uuid)
{
	struct avrcp_data *data;
	const sdp_record_t *rec;
	sdp_list_t *list;
	sdp_profile_desc_t *desc;

	data = g_new0(struct avrcp_data, 1);

	rec = btd_device_get_record(session->dev, uuid);
	if (rec == NULL)
		return data;

	if (sdp_get_profile_descs(rec, &list) == 0) {
		desc = list->data;
		data->version = desc->version;
	}

	sdp_get_int_attr(rec, SDP_ATTR_SUPPORTED_FEATURES, &data->features);
	sdp_list_free(list, free);

	return data;
}