GF_Err gf_isom_read_null_terminated_string(GF_Box *s, GF_BitStream *bs, u64 size, char **out_str)
{
	u32 len=10;
	u32 i=0;

	*out_str = gf_malloc(sizeof(char)*len);
	if (! *out_str) return GF_OUT_OF_MEM;

	if (!s->size) {
		*out_str[0] = 0;
		return GF_OK;
	}

	while (1) {
		ISOM_DECREASE_SIZE(s, 1 );
		(*out_str)[i] = gf_bs_read_u8(bs);
		if (!(*out_str)[i]) break;
		i++;
		if (i==len) {
			len += 10;
			*out_str = gf_realloc(*out_str, sizeof(char)*len);
		}
		if (gf_bs_available(bs) == 0) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] missing null character in null terminated string\n"));
			(*out_str)[i] = 0;
			return GF_OK;
		}
		if (i >= size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] string bigger than container, probably missing null character\n"));
			(*out_str)[i] = 0;
			return GF_OK;
		}
	}
	return GF_OK;
}