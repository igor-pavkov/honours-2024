void unkn_box_del(GF_Box *s)
{
	GF_UnknownBox *ptr = (GF_UnknownBox *) s;
	if (!s) return;
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}