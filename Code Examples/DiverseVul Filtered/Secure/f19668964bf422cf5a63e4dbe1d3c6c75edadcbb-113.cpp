void iKMS_box_del(GF_Box *s)
{
	GF_ISMAKMSBox *ptr = (GF_ISMAKMSBox *)s;
	if (ptr == NULL) return;
	if (ptr->URI) gf_free(ptr->URI);
	gf_free(ptr);
}