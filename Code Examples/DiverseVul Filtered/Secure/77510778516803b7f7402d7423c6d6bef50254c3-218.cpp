void emsg_box_del(GF_Box *s)
{
	GF_EventMessageBox *ptr = (GF_EventMessageBox *) s;
	if (ptr == NULL) return;
	if (ptr->scheme_id_uri) gf_free(ptr->scheme_id_uri);
	if (ptr->value) gf_free(ptr->value);
	if (ptr->message_data) gf_free(ptr->message_data);
	gf_free(ptr);
}