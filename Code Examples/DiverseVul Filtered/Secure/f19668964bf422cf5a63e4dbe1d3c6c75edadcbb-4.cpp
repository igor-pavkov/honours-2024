void iSLT_box_del(GF_Box *s)
{
	GF_ISMACrypSaltBox *ptr = (GF_ISMACrypSaltBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}