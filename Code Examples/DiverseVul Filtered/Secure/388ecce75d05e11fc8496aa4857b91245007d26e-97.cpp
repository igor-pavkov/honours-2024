GF_Err mdhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;

	if (ptr->version == 1) {
		ISOM_DECREASE_SIZE(ptr, 28)
		ptr->creationTime = gf_bs_read_u64(bs);
		ptr->modificationTime = gf_bs_read_u64(bs);
		ptr->timeScale = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 16)
		ptr->creationTime = gf_bs_read_u32(bs);
		ptr->modificationTime = gf_bs_read_u32(bs);
		ptr->timeScale = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u32(bs);
	}
	if (!ptr->timeScale) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Media header timescale is 0 - defaulting to 90000\n" ));
		ptr->timeScale = 90000;
	}

	ptr->original_duration = ptr->duration;

	ISOM_DECREASE_SIZE(ptr, 4)
	//our padding bit
	gf_bs_read_int(bs, 1);
	//the spec is unclear here, just says "the value 0 is interpreted as undetermined"
	ptr->packedLanguage[0] = gf_bs_read_int(bs, 5);
	ptr->packedLanguage[1] = gf_bs_read_int(bs, 5);
	ptr->packedLanguage[2] = gf_bs_read_int(bs, 5);
	//but before or after compaction ?? We assume before
	if (ptr->packedLanguage[0] || ptr->packedLanguage[1] || ptr->packedLanguage[2]) {
		ptr->packedLanguage[0] += 0x60;
		ptr->packedLanguage[1] += 0x60;
		ptr->packedLanguage[2] += 0x60;
	} else {
		ptr->packedLanguage[0] = 'u';
		ptr->packedLanguage[1] = 'n';
		ptr->packedLanguage[2] = 'd';
	}
	ptr->reserved = gf_bs_read_u16(bs);
	return GF_OK;
}