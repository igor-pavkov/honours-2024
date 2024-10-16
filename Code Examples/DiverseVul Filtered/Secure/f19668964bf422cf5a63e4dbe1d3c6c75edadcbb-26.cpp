void aeib_box_del(GF_Box *s)
{
	GF_AdobeEncryptionInfoBox *ptr = (GF_AdobeEncryptionInfoBox*)s;
	if (!ptr) return;
	if (ptr->enc_algo) gf_free(ptr->enc_algo);
	gf_free(ptr);
}