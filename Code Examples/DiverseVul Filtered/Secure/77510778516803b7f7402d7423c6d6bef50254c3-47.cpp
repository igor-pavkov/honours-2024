GF_Err pdin_box_size(GF_Box *s)
{
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox *)s;
	ptr->size += 8*ptr->count;
	return GF_OK;
}