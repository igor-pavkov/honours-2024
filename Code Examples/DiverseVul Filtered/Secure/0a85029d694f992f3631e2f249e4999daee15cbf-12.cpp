void btrt_box_del(GF_Box *s)
{
	GF_BitRateBox *ptr = (GF_BitRateBox *)s;
	if (ptr) gf_free(ptr);
}