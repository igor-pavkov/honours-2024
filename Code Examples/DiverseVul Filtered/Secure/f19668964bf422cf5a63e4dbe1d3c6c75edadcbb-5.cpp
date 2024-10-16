GF_Err flxs_box_size(GF_Box *s)
{
	GF_AdobeFlashAccessParamsBox *ptr = (GF_AdobeFlashAccessParamsBox*)s;
	if (ptr->metadata)
		ptr->size += strlen(ptr->metadata) + 1;
	return GF_OK;
}