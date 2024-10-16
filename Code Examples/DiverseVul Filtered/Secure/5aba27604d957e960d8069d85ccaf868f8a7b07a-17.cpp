GF_Err WriteToFile(GF_ISOFile *movie, Bool for_fragments)
{
	MovieWriter mw;
	GF_Err e = GF_OK;
	if (!movie) return GF_BAD_PARAM;

	if (movie->openMode == GF_ISOM_OPEN_READ) return GF_BAD_PARAM;

	e = gf_isom_insert_copyright(movie);
	if (e) return e;

	memset(&mw, 0, sizeof(mw));
	mw.movie = movie;


	if (movie->moov) {
		u32 i;
		GF_TrackBox *trak;
		if (gf_sys_is_test_mode()) {
			movie->moov->mvhd->creationTime = 0;
			movie->moov->mvhd->modificationTime = 0;
		}
		i=0;
		while ( (trak = gf_list_enum(movie->moov->trackList, &i))) {
			if (gf_sys_is_test_mode()) {
				trak->Header->creationTime = 0;
				trak->Header->modificationTime = 0;
				if (trak->Media->handler->nameUTF8 && strstr(trak->Media->handler->nameUTF8, "@GPAC")) {
					gf_free(trak->Media->handler->nameUTF8);
					trak->Media->handler->nameUTF8 = gf_strdup("MediaHandler");
				}
				trak->Media->mediaHeader->creationTime = 0;
				trak->Media->mediaHeader->modificationTime = 0;
			}
			if (trak->chunk_cache) {
				gf_isom_flush_chunk(trak, GF_TRUE);
			}
		}
	}
	//capture mode: we don't need a new bitstream
	if (movie->openMode == GF_ISOM_OPEN_WRITE) {
		if (!strcmp(movie->fileName, "_gpac_isobmff_redirect")) {
			GF_BitStream *bs, *moov_bs=NULL;
			u64 mdat_end = gf_bs_get_position(movie->editFileMap->bs);
			u64 mdat_start = movie->mdat->bsOffset;
			u64 mdat_size = mdat_end - mdat_start;

			if (for_fragments) {
				if (!movie->on_block_out) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Missing output block callback, cannot write\n"));
					return GF_BAD_PARAM;
				}
				bs = gf_bs_new_cbk(movie->on_block_out, movie->on_block_out_usr_data, movie->on_block_out_block_size);
				e = WriteFlat(&mw, 0, bs, GF_TRUE, GF_TRUE, NULL);
				movie->fragmented_file_pos = gf_bs_get_position(bs);
				gf_bs_del(bs);
				return e;
			}
			//seek at end in case we had a read of the file
			gf_bs_seek(movie->editFileMap->bs, gf_bs_get_size(movie->editFileMap->bs) );

			if ((movie->storageMode==GF_ISOM_STORE_FASTSTART) && mdat_start && mdat_size) {
				moov_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
			}
			//write as non-seekable
			e = WriteFlat(&mw, 0, movie->editFileMap->bs, GF_TRUE, GF_FALSE, moov_bs);

			movie->fragmented_file_pos = gf_bs_get_position(movie->editFileMap->bs);

			if (mdat_start && mdat_size) {
				u8 data[16];
				if (!movie->on_block_out) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Missing output block patch callback, cannot patch mdat size in flat storage\n"));
					return GF_BAD_PARAM;
				}

				//create a patch packet for mdat covering out 16 bytes (cf FlushCapture)
				bs = gf_bs_new(data, 16, GF_BITSTREAM_WRITE);
				gf_bs_write_u32(bs, (mdat_size>0xFFFFFFFF) ? 1 : (u32) mdat_size);
				gf_bs_write_u32(bs, GF_ISOM_BOX_TYPE_MDAT);
				if  (mdat_size>0xFFFFFFFF)
					gf_bs_write_u64(bs, mdat_size);
				else
					gf_bs_write_u64(bs, 0);
				gf_bs_del(bs);
				movie->on_block_patch(movie->on_block_out_usr_data, data, 16, mdat_start, GF_FALSE);
			}

			if (moov_bs) {
				u8 *moov_data;
				u32 moov_size;

				gf_bs_get_content(moov_bs, &moov_data, &moov_size);
				gf_bs_del(moov_bs);

				movie->on_block_patch(movie->on_block_out_usr_data, moov_data, moov_size, mdat_start, GF_TRUE);
				gf_free(moov_data);
			}
		} else {
			GF_BitStream *moov_bs = NULL;
			if ((movie->storageMode==GF_ISOM_STORE_STREAMABLE) || (movie->storageMode==GF_ISOM_STORE_FASTSTART) ) {
				moov_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
			}
			e = WriteFlat(&mw, 0, movie->editFileMap->bs, GF_FALSE, GF_FALSE, moov_bs);
			if (moov_bs) {
				u8 *moov_data;
				u32 moov_size;

				gf_bs_get_content(moov_bs, &moov_data, &moov_size);
				gf_bs_del(moov_bs);
				if (!e)
					e = gf_bs_insert_data(movie->editFileMap->bs, moov_data, moov_size, movie->mdat->bsOffset);
					
				gf_free(moov_data);
			}
		}
	} else {
		FILE *stream=NULL;
		Bool is_stdout = GF_FALSE;
		GF_BitStream *bs=NULL;
		if (!strcmp(movie->finalName, "_gpac_isobmff_redirect")) {
			if (!movie->on_block_out) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] Missing output block callback, cannot write\n"));
				return GF_BAD_PARAM;
			}
			bs = gf_bs_new_cbk(movie->on_block_out, movie->on_block_out_usr_data, movie->on_block_out_block_size);
			is_stdout = GF_TRUE;
		} else {
			if (!strcmp(movie->finalName, "std"))
				is_stdout = GF_TRUE;

			//OK, we need a new bitstream
			stream = is_stdout ? stdout : gf_fopen(movie->finalName, "w+b");
			if (!stream)
				return GF_IO_ERR;
			bs = gf_bs_from_file(stream, GF_BITSTREAM_WRITE);
		}
		if (!bs) {
			if (!is_stdout)
				gf_fclose(stream);
			return GF_OUT_OF_MEM;
		}

		switch (movie->storageMode) {
		case GF_ISOM_STORE_TIGHT:
		case GF_ISOM_STORE_INTERLEAVED:
			e = WriteInterleaved(&mw, bs, 0);
			break;
		case GF_ISOM_STORE_DRIFT_INTERLEAVED:
			e = WriteInterleaved(&mw, bs, 1);
			break;
		case GF_ISOM_STORE_STREAMABLE:
			e = WriteFlat(&mw, 1, bs, is_stdout, GF_FALSE, NULL);
			break;
		default:
			e = WriteFlat(&mw, 0, bs, is_stdout, GF_FALSE, NULL);
			break;
		}

		gf_bs_del(bs);
		if (!is_stdout)
			gf_fclose(stream);
	}
	if (mw.buffer) gf_free(mw.buffer);
	if (mw.nb_done<mw.total_samples) {
		mw.nb_done = mw.total_samples;
		muxer_report_progress(&mw);
	}
	return e;
}