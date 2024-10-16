GF_Err emsg_box_size(GF_Box *s)
{
	GF_EventMessageBox *ptr = (GF_EventMessageBox*) s;

	if (ptr->version) {
		ptr->size += 20;
	} else {
		ptr->size += 16;
	}
	ptr->size+=2; //1 NULL-terminated strings
	if (ptr->scheme_id_uri) ptr->size += strlen(ptr->scheme_id_uri);
	if (ptr->value) ptr->size += strlen(ptr->value);
	if (ptr->message_data)
		ptr->size += ptr->message_data_size;

	return GF_OK;
}