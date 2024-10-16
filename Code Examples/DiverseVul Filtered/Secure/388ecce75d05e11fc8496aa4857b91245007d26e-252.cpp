void trak_box_del(GF_Box *s)
{
#ifndef GPAC_DISABLE_ISOM_WRITE
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	if (ptr->chunk_cache)
		gf_bs_del(ptr->chunk_cache);
#endif
	gf_free(s);
}