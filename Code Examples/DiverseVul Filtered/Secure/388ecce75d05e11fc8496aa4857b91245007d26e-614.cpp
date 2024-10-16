void ctts_box_del(GF_Box *s)
{
	GF_CompositionOffsetBox *ptr = (GF_CompositionOffsetBox *)s;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}