void uuid_box_del(GF_Box *s)
{
	GF_UnknownUUIDBox *ptr = (GF_UnknownUUIDBox *) s;
	if (!s) return;
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}