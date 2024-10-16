void payt_box_del(GF_Box *s)
{
	GF_PAYTBox *payt = (GF_PAYTBox *)s;
	if (payt->payloadString) gf_free(payt->payloadString);
	gf_free(payt);
}