static void lsr_read_byte_align_string(GF_LASeRCodec *lsr, char **str, const char *name)
{
	u32 len;
	gf_bs_align(lsr->bs);
	len = lsr_read_vluimsbf8(lsr, "len");
	if (str) {
		if (*str) gf_free(*str);
		*str = NULL;
		if (len) {
			if (len > gf_bs_available(lsr->bs) ) {
				lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
				return;
			}
			*str = (char*)gf_malloc(sizeof(char)*(len+1));
			if (!*str) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}
			gf_bs_read_data(lsr->bs, *str, len);
			(*str) [len] = 0;
		}
	} else {
		if (len > gf_bs_available(lsr->bs) ) {
			lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
			return;
		}
		while (len) {
			gf_bs_read_int(lsr->bs, 8);
			len--;
		}
	}
	GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[LASeR] %s\t\t%d\t\t%s\n", name, 8*len, str ? *str : ""));
}