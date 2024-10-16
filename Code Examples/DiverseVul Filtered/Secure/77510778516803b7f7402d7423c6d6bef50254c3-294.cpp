void trik_box_del(GF_Box *s)
{
	GF_TrickPlayBox *ptr = (GF_TrickPlayBox *) s;
	if (ptr == NULL) return;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}