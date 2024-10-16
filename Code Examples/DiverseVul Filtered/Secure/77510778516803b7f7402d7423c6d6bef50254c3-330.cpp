void sidx_box_del(GF_Box *s)
{
	GF_SegmentIndexBox *ptr = (GF_SegmentIndexBox *) s;
	if (ptr == NULL) return;
	if (ptr->refs) gf_free(ptr->refs);
	gf_free(ptr);
}