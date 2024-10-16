void co64_box_del(GF_Box *s)
{
	GF_ChunkLargeOffsetBox *ptr;
	ptr = (GF_ChunkLargeOffsetBox *) s;
	if (ptr == NULL) return;
	if (ptr->offsets) gf_free(ptr->offsets);
	gf_free(ptr);
}