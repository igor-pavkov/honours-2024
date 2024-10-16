GF_Err gf_laser_decoder_remove_stream(GF_LASeRCodec *codec, u16 ESID)
{
	u32 i, count;
	count = gf_list_count(codec->streamInfo);
	for (i=0; i<count; i++) {
		LASeRStreamInfo *ptr = (LASeRStreamInfo *) gf_list_get(codec->streamInfo, i);
		if (ptr->ESID==ESID) {
			gf_free(ptr);
			gf_list_rem(codec->streamInfo, i);
			return GF_OK;
		}
	}
	return GF_BAD_PARAM;
}