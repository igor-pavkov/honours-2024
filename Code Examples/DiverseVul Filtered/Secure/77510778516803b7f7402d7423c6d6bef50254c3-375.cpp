void reftype_box_del(GF_Box *s)
{
	GF_TrackReferenceTypeBox *ptr = (GF_TrackReferenceTypeBox *)s;
	if (!ptr) return;
	if (ptr->trackIDs) gf_free(ptr->trackIDs);
	gf_free(ptr);
}