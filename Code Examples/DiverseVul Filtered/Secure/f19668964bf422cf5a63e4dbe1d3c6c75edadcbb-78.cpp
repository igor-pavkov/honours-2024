GF_Err flxs_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_AdobeFlashAccessParamsBox *ptr = (GF_AdobeFlashAccessParamsBox*)s;
	u32 len;

	len = (u32) ptr->size;
	if (len) {
		ptr->metadata = (char *)gf_malloc(len*sizeof(char));
		if (!ptr->metadata) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->metadata, len);
	}
	return GF_OK;
}