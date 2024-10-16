GF_Err tfhd_box_size(GF_Box *s)
{
	GF_TrackFragmentHeaderBox *ptr = (GF_TrackFragmentHeaderBox *)s;
	ptr->size += 4;

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRAF_BASE_OFFSET) ptr->size += 8;
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DESC) ptr->size += 4;
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DUR) ptr->size += 4;
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_SIZE) ptr->size += 4;
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_FLAGS) ptr->size += 4;
	return GF_OK;
}