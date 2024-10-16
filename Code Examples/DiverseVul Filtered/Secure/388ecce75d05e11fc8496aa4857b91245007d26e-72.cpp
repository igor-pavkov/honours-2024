void stco_box_del(GF_Box *s)
{
	GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;
	if (ptr == NULL) return;
	if (ptr->offsets) gf_free(ptr->offsets);
	gf_free(ptr);
}