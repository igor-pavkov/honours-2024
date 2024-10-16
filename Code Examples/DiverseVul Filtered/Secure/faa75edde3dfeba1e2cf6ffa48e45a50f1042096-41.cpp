static void lsr_read_any_uri(GF_LASeRCodec *lsr, XMLRI *iri, const char *name)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "hasUri");
	if (val) {
		char *s = NULL;
		iri->type=XMLRI_STRING;
		if (iri->string) {
			gf_free(iri->string);
			iri->string = NULL;
		}
		lsr_read_byte_align_string(lsr, &s, "uri");
		GF_LSR_READ_INT(lsr, val, 1, "hasData");
		if (!val) {
			iri->string = s;
		} else {
			u32 len_rad, len;
			len = lsr_read_vluimsbf5(lsr, "len");
			if (len > gf_bs_available(lsr->bs)) {
				lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
				return;
			}
			len_rad = s ? (u32) strlen(s) : 0;
			iri->string = (char*)gf_malloc(sizeof(char)*(len_rad+1+len+1));
			if (!iri->string) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}

			iri->string[0] = 0;
			if (s) {
				strcpy(iri->string, s);
				gf_free(s);
			}
			strcat(iri->string, ",");
			gf_bs_read_data(lsr->bs, iri->string + len_rad + 1, len);
			iri->string[len_rad + 1 + len] = 0;
		}
	}
	GF_LSR_READ_INT(lsr, val, 1, "hasID");
	if (val) lsr_read_codec_IDREF(lsr, iri, "idref");

	GF_LSR_READ_INT(lsr, val, 1, "hasStreamID");
	if (val) {
		iri->type = XMLRI_STREAMID;
		iri->lsr_stream_id = lsr_read_vluimsbf5(lsr, name);
		GF_LSR_READ_INT(lsr, val, 1, "reserved");
		if (val) {
			u32 len = lsr_read_vluimsbf5(lsr, "len");
			GF_LSR_READ_INT(lsr, val, len, "reserved");
		}
	}
}