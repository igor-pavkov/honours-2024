GF_Err stss_box_size(GF_Box *s)
{
	GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;
	ptr->size += 4 + (4 * ptr->nb_entries);
	return GF_OK;
}