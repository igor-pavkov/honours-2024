void snro_box_del(GF_Box *s)
{
	GF_SeqOffHintEntryBox *snro = (GF_SeqOffHintEntryBox *)s;
	gf_free(snro);
}