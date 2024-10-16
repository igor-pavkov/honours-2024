GF_Err cprt_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_CopyrightBox *ptr = (GF_CopyrightBox *)s;

	ISOM_DECREASE_SIZE(ptr, 2);
	gf_bs_read_int(bs, 1);
	//the spec is unclear here, just says "the value 0 is interpreted as undetermined"
	ptr->packedLanguageCode[0] = gf_bs_read_int(bs, 5);
	ptr->packedLanguageCode[1] = gf_bs_read_int(bs, 5);
	ptr->packedLanguageCode[2] = gf_bs_read_int(bs, 5);

	//but before or after compaction ?? We assume before
	if (ptr->packedLanguageCode[0] || ptr->packedLanguageCode[1] || ptr->packedLanguageCode[2]) {
		ptr->packedLanguageCode[0] += 0x60;
		ptr->packedLanguageCode[1] += 0x60;
		ptr->packedLanguageCode[2] += 0x60;
	} else {
		ptr->packedLanguageCode[0] = 'u';
		ptr->packedLanguageCode[1] = 'n';
		ptr->packedLanguageCode[2] = 'd';
	}
	if (ptr->size) {
		u32 bytesToRead = (u32) ptr->size;
		ptr->notice = (char*)gf_malloc((bytesToRead+1) * sizeof(char));
		if (ptr->notice == NULL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->notice, bytesToRead);
		ptr->notice[bytesToRead] = 0;
	}
	return GF_OK;
}