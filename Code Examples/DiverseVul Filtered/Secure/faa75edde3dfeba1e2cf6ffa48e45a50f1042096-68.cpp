static u32 lsr_read_vluimsbf5(GF_LASeRCodec *lsr, const char *name)
{
	u32 nb_words = 0;
	u32 nb_tot, nb_bits, val;

	while (gf_bs_read_int(lsr->bs, 1)) nb_words++;
	nb_words++;
	nb_tot = nb_words;
	nb_bits = nb_words*4;
	nb_tot += nb_bits;
	val = gf_bs_read_int(lsr->bs, nb_bits);
	if (name) GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[LASeR] %s\t\t%d\t\t%d\n", name, nb_tot, val));
	return val;
}