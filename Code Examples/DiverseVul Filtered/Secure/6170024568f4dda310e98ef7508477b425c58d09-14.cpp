void wide_box_del(GF_Box *s)
{
	if (s == NULL) return;
	gf_free(s);
}