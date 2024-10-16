void fecr_box_del(GF_Box *s)
{
	FECReservoirBox *ptr = (FECReservoirBox *)s;
	if (ptr == NULL) return;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}