static LASeRStreamInfo *lsr_get_stream(GF_LASeRCodec *codec, u16 ESID)
{
	u32 i=0;
	LASeRStreamInfo *ptr;
	while ((ptr = (LASeRStreamInfo *)gf_list_enum(codec->streamInfo, &i))) {
		if (!ESID || (ptr->ESID==ESID)) return ptr;
	}
	return NULL;
}