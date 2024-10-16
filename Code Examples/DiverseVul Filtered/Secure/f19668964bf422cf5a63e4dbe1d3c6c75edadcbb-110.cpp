GF_Err aeib_box_size(GF_Box *s)
{
	GF_AdobeEncryptionInfoBox *ptr = (GF_AdobeEncryptionInfoBox*)s;
	if (ptr->enc_algo)
		ptr->size += strlen(ptr->enc_algo) + 1;
	ptr->size += 1; //KeyLength
	return GF_OK;
}