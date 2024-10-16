GF_Err mvhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	if (ptr->version == 1) {
		ISOM_DECREASE_SIZE(ptr, 28);
		ptr->creationTime = gf_bs_read_u64(bs);
		ptr->modificationTime = gf_bs_read_u64(bs);
		ptr->timeScale = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 16);
		ptr->creationTime = gf_bs_read_u32(bs);
		ptr->modificationTime = gf_bs_read_u32(bs);
		ptr->timeScale = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u32(bs);
	}
	if (!ptr->timeScale) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Movie header timescale is invalid (0) - defaulting to 600\n" ));
		ptr->timeScale = 600;
	}
	ISOM_DECREASE_SIZE(ptr, 80);
	ptr->preferredRate = gf_bs_read_u32(bs);
	ptr->preferredVolume = gf_bs_read_u16(bs);
	gf_bs_read_data(bs, ptr->reserved, 10);
	ptr->matrixA = gf_bs_read_u32(bs);
	ptr->matrixB = gf_bs_read_u32(bs);
	ptr->matrixU = gf_bs_read_u32(bs);
	ptr->matrixC = gf_bs_read_u32(bs);
	ptr->matrixD = gf_bs_read_u32(bs);
	ptr->matrixV = gf_bs_read_u32(bs);
	ptr->matrixX = gf_bs_read_u32(bs);
	ptr->matrixY = gf_bs_read_u32(bs);
	ptr->matrixW = gf_bs_read_u32(bs);
	ptr->previewTime = gf_bs_read_u32(bs);
	ptr->previewDuration = gf_bs_read_u32(bs);
	ptr->posterTime = gf_bs_read_u32(bs);
	ptr->selectionTime = gf_bs_read_u32(bs);
	ptr->selectionDuration = gf_bs_read_u32(bs);
	ptr->currentTime = gf_bs_read_u32(bs);
	ptr->nextTrackID = gf_bs_read_u32(bs);
	ptr->original_duration = ptr->duration;
	return GF_OK;
}