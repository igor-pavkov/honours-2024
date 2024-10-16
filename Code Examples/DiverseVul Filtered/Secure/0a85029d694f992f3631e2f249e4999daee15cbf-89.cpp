void SmDm_box_del(GF_Box *a)
{
	GF_SMPTE2086MasteringDisplayMetadataBox *p = (GF_SMPTE2086MasteringDisplayMetadataBox *)a;
	gf_free(p);
}