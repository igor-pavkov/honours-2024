GF_Err WriteSample(MovieWriter *mw, u32 size, u64 offset, u8 isEdited, GF_BitStream *bs, u32 nb_samp)
{
	GF_DataMap *map;
	u32 bytes;

	if (!size) return GF_OK;

	if (size>mw->alloc_size) {
		mw->buffer = (char*)gf_realloc(mw->buffer, size);
		mw->alloc_size = size;
	}

	if (!mw->buffer) return GF_OUT_OF_MEM;

	if (isEdited) {
		map = mw->movie->editFileMap;
	} else {
		map = mw->movie->movieFileMap;
	}
	//get the payload...
	bytes = gf_isom_datamap_get_data(map, mw->buffer, size, offset);
	if (bytes != size)
		return GF_IO_ERR;
	//write it to our stream...
	bytes = gf_bs_write_data(bs, mw->buffer, size);
	if (bytes != size)
		return GF_IO_ERR;

	mw->nb_done+=nb_samp;
	muxer_report_progress(mw);
	return GF_OK;
}