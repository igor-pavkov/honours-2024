GF_Err dvcC_box_size(GF_Box *s)
{
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox *)s;

	ptr->size += 24;
	return GF_OK;
}