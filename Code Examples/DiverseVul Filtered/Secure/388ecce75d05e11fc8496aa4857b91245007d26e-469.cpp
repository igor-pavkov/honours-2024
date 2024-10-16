GF_Err mp4s_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MPEGSampleEntryBox *ptr = (GF_MPEGSampleEntryBox *)s;
	s->size += 8;
	gf_isom_check_position(s, (GF_Box *)ptr->esd, &pos);
    return GF_OK;
}