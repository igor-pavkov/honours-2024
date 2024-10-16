void dac3_box_del(GF_Box *s)
{
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;
	gf_free(ptr);
}