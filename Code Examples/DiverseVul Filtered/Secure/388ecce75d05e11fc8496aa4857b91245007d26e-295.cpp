GF_Err mvhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->version == 1) {
		gf_bs_write_u64(bs, ptr->creationTime);
		gf_bs_write_u64(bs, ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->timeScale);
		gf_bs_write_u64(bs, ptr->duration);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->creationTime);
		gf_bs_write_u32(bs, (u32) ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->timeScale);
		gf_bs_write_u32(bs, (u32) ptr->duration);
	}
	gf_bs_write_u32(bs, ptr->preferredRate);
	gf_bs_write_u16(bs, ptr->preferredVolume);
	gf_bs_write_data(bs, ptr->reserved, 10);
	gf_bs_write_u32(bs, ptr->matrixA);
	gf_bs_write_u32(bs, ptr->matrixB);
	gf_bs_write_u32(bs, ptr->matrixU);
	gf_bs_write_u32(bs, ptr->matrixC);
	gf_bs_write_u32(bs, ptr->matrixD);
	gf_bs_write_u32(bs, ptr->matrixV);
	gf_bs_write_u32(bs, ptr->matrixX);
	gf_bs_write_u32(bs, ptr->matrixY);
	gf_bs_write_u32(bs, ptr->matrixW);
	gf_bs_write_u32(bs, ptr->previewTime);
	gf_bs_write_u32(bs, ptr->previewDuration);
	gf_bs_write_u32(bs, ptr->posterTime);
	gf_bs_write_u32(bs, ptr->selectionTime);
	gf_bs_write_u32(bs, ptr->selectionDuration);
	gf_bs_write_u32(bs, ptr->currentTime);
	gf_bs_write_u32(bs, ptr->nextTrackID);
	return GF_OK;
}