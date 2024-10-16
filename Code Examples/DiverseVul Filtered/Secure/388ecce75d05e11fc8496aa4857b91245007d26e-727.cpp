void tims_box_del(GF_Box *s)
{
	GF_TSHintEntryBox *tims = (GF_TSHintEntryBox *)s;
	gf_free(tims);
}