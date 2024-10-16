void extr_box_del(GF_Box *s)
{
	GF_ExtraDataBox *ptr = (GF_ExtraDataBox *)s;
	if (ptr == NULL) return;
	if (ptr->feci) gf_isom_box_del((GF_Box*)ptr->feci);
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}