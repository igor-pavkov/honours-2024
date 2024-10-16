void hinf_box_del(GF_Box *s)
{
	GF_HintInfoBox *hinf = (GF_HintInfoBox *)s;
	gf_free(hinf);
}