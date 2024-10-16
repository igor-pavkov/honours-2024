void t2p_read_tiff_size_tile(T2P* t2p, TIFF* input, ttile_t tile){

	uint64* tbc = NULL;
	uint16 edge=0;
#ifdef JPEG_SUPPORT
	unsigned char* jpt;
#endif
        uint64 k;

	edge |= t2p_tile_is_right_edge(t2p->tiff_tiles[t2p->pdf_page], tile);
	edge |= t2p_tile_is_bottom_edge(t2p->tiff_tiles[t2p->pdf_page], tile);
	
	if(t2p->pdf_transcode==T2P_TRANSCODE_RAW){
		if(edge
#if defined(JPEG_SUPPORT) || defined(OJPEG_SUPPORT)
		&& !(t2p->pdf_compression==T2P_COMPRESS_JPEG)
#endif
		){
			t2p->tiff_datasize=TIFFTileSize(input);
			if (t2p->tiff_datasize == 0) {
				/* Assume we had overflow inside TIFFTileSize */
				t2p->t2p_error = T2P_ERR_ERROR;
			}
			return;
		} else {
			TIFFGetField(input, TIFFTAG_TILEBYTECOUNTS, &tbc);
			k=tbc[tile];
#ifdef OJPEG_SUPPORT
			if(t2p->tiff_compression==COMPRESSION_OJPEG){
				k = checkAdd64(k, 2048, t2p);
			}
#endif
#ifdef JPEG_SUPPORT
			if(t2p->tiff_compression==COMPRESSION_JPEG) {
				uint32 count = 0;
				if(TIFFGetField(input, TIFFTAG_JPEGTABLES, &count, &jpt)!=0){
					if(count > 4){
						k = checkAdd64(k, count, t2p);
						k -= 2; /* don't use EOI of header or SOI of tile */
					}
				}
			}
#endif
			t2p->tiff_datasize = (tsize_t) k;
			if ((uint64) t2p->tiff_datasize != k) {
				TIFFError(TIFF2PDF_MODULE, "Integer overflow");
				t2p->t2p_error = T2P_ERR_ERROR;
			}
			return;
		}
	}
	k = TIFFTileSize(input);
	if(t2p->tiff_planar==PLANARCONFIG_SEPARATE){
		k = checkMultiply64(k, t2p->tiff_samplesperpixel, t2p);
	}
	if (k == 0) {
		/* Assume we had overflow inside TIFFTileSize */
		t2p->t2p_error = T2P_ERR_ERROR;
	}

	t2p->tiff_datasize = (tsize_t) k;
	if ((uint64) t2p->tiff_datasize != k) {
		TIFFError(TIFF2PDF_MODULE, "Integer overflow");
		t2p->t2p_error = T2P_ERR_ERROR;
	}

	return;
}