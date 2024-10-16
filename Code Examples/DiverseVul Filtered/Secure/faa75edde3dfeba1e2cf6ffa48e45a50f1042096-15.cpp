static void lsr_read_extension(GF_LASeRCodec *lsr, const char *name)
{
	u32 len = lsr_read_vluimsbf5(lsr, name);
#if 0
	*out_data = gf_malloc(sizeof(char)*len);
	gf_bs_read_data(lsr->bs, *out_data, len);
	*out_len = len;
#else
	while (len && gf_bs_available(lsr->bs) ) {
		gf_bs_read_int(lsr->bs, 8);
		len--;
	}
	if (len) lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
#endif
}