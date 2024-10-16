GF_Err stsg_box_size(GF_Box *s)
{
	GF_SubTrackSampleGroupBox *ptr = (GF_SubTrackSampleGroupBox *)s;
	ptr->size += 6 + 4 * ptr->nb_groups;
	return GF_OK;
}