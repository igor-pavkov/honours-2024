void dOps_box_del(GF_Box *s)
{
	GF_OpusSpecificBox *ptr = (GF_OpusSpecificBox *)s;
	if (ptr) gf_free(ptr);
}