GF_Err trep_box_size(GF_Box *s)
{
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *)s;
	ptr->size += 4;
	return GF_OK;
}