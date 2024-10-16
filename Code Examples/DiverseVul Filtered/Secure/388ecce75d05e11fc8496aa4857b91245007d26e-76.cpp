void padb_box_del(GF_Box *s)
{
	GF_PaddingBitsBox *ptr = (GF_PaddingBitsBox *) s;
	if (ptr == NULL) return;
	if (ptr->padbits) gf_free(ptr->padbits);
	gf_free(ptr);
}