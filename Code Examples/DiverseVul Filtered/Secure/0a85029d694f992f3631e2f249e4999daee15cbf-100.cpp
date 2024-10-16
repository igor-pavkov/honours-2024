GF_Err SmDm_box_size(GF_Box *s)
{
	GF_SMPTE2086MasteringDisplayMetadataBox *p = (GF_SMPTE2086MasteringDisplayMetadataBox*)s;
	p->size += 24;
	return GF_OK;
}