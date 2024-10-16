void dref_box_del(GF_Box *s)
{
	GF_DataReferenceBox *ptr = (GF_DataReferenceBox *) s;
	if (ptr == NULL) return;
	gf_free(ptr);
}