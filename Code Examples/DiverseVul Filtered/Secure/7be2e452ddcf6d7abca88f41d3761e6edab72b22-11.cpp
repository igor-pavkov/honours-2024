void t2p_read_tiff_init(T2P* t2p, TIFF* input){

	tdir_t directorycount=0;
	tdir_t i=0;
	uint16 pagen=0;
	uint16 paged=0;
	uint16 xuint16=0;
	uint16 tiff_transferfunctioncount=0;
	uint16* tiff_transferfunction[3];

	directorycount=TIFFNumberOfDirectories(input);
	if(directorycount > TIFF_DIR_MAX) {
		TIFFError(
			TIFF2PDF_MODULE,
			"TIFF contains too many directories, %s",
			TIFFFileName(input));
		t2p->t2p_error = T2P_ERR_ERROR;
		return;
	}
	t2p->tiff_pages = (T2P_PAGE*) _TIFFmalloc(TIFFSafeMultiply(tmsize_t,directorycount,sizeof(T2P_PAGE)));
	if(t2p->tiff_pages==NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't allocate " TIFF_SIZE_FORMAT " bytes of memory for tiff_pages array, %s", 
			(TIFF_SIZE_T) directorycount * sizeof(T2P_PAGE), 
			TIFFFileName(input));
		t2p->t2p_error = T2P_ERR_ERROR;
		return;
	}
	_TIFFmemset( t2p->tiff_pages, 0x00, directorycount * sizeof(T2P_PAGE));
	t2p->tiff_tiles = (T2P_TILES*) _TIFFmalloc(TIFFSafeMultiply(tmsize_t,directorycount,sizeof(T2P_TILES)));
	if(t2p->tiff_tiles==NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't allocate " TIFF_SIZE_FORMAT " bytes of memory for tiff_tiles array, %s", 
			(TIFF_SIZE_T) directorycount * sizeof(T2P_TILES), 
			TIFFFileName(input));
		t2p->t2p_error = T2P_ERR_ERROR;
		return;
	}
	_TIFFmemset( t2p->tiff_tiles, 0x00, directorycount * sizeof(T2P_TILES));
	for(i=0;i<directorycount;i++){
		uint32 subfiletype = 0;
		
		if(!TIFFSetDirectory(input, i)){
			TIFFError(
				TIFF2PDF_MODULE, 
				"Can't set directory %u of input file %s", 
				i,
				TIFFFileName(input));
			t2p->t2p_error = T2P_ERR_ERROR;
			return;
		}
		if(TIFFGetField(input, TIFFTAG_PAGENUMBER, &pagen, &paged)){
			if((pagen>paged) && (paged != 0)){
				t2p->tiff_pages[t2p->tiff_pagecount].page_number = 
					paged;
			} else {
				t2p->tiff_pages[t2p->tiff_pagecount].page_number = 
					pagen;
			}
			goto ispage2;
		}
		if(TIFFGetField(input, TIFFTAG_SUBFILETYPE, &subfiletype)){
			if ( ((subfiletype & FILETYPE_PAGE) != 0)
                             || (subfiletype == 0)){
				goto ispage;
			} else {
				goto isnotpage;
			}
		}
		if(TIFFGetField(input, TIFFTAG_OSUBFILETYPE, &subfiletype)){
			if ((subfiletype == OFILETYPE_IMAGE) 
				|| (subfiletype == OFILETYPE_PAGE)
				|| (subfiletype == 0) ){
				goto ispage;
			} else {
				goto isnotpage;
			}
		}
		ispage:
		t2p->tiff_pages[t2p->tiff_pagecount].page_number=t2p->tiff_pagecount;
		ispage2:
		t2p->tiff_pages[t2p->tiff_pagecount].page_directory=i;
		if(TIFFIsTiled(input)){
			t2p->tiff_pages[t2p->tiff_pagecount].page_tilecount = 
				TIFFNumberOfTiles(input);
		}
		t2p->tiff_pagecount++;
		isnotpage:
		(void)0;
	}
	
	qsort((void*) t2p->tiff_pages, t2p->tiff_pagecount,
              sizeof(T2P_PAGE), t2p_cmp_t2p_page);

	for(i=0;i<t2p->tiff_pagecount;i++){
		t2p->pdf_xrefcount += 5;
		TIFFSetDirectory(input, t2p->tiff_pages[i].page_directory );
		if((TIFFGetField(input, TIFFTAG_PHOTOMETRIC, &xuint16)
                    && (xuint16==PHOTOMETRIC_PALETTE))
		   || TIFFGetField(input, TIFFTAG_INDEXED, &xuint16)) {
			t2p->tiff_pages[i].page_extra++;
			t2p->pdf_xrefcount++;
		}
#ifdef ZIP_SUPPORT
		if (TIFFGetField(input, TIFFTAG_COMPRESSION, &xuint16)) {
                        if( (xuint16== COMPRESSION_DEFLATE ||
                             xuint16== COMPRESSION_ADOBE_DEFLATE) && 
                            ((t2p->tiff_pages[i].page_tilecount != 0) 
                             || TIFFNumberOfStrips(input)==1) &&
                            (t2p->pdf_nopassthrough==0)	){
                                if(t2p->pdf_minorversion<2){t2p->pdf_minorversion=2;}
                        }
                }
#endif
		if (TIFFGetField(input, TIFFTAG_TRANSFERFUNCTION,
                                 &(tiff_transferfunction[0]),
                                 &(tiff_transferfunction[1]),
                                 &(tiff_transferfunction[2]))) {

                        if((tiff_transferfunction[1] != (uint16*) NULL) &&
                           (tiff_transferfunction[2] != (uint16*) NULL)
                          ) {
                            tiff_transferfunctioncount=3;
                        } else {
                            tiff_transferfunctioncount=1;
                        }
                } else {
			tiff_transferfunctioncount=0;
		}

                if (i > 0){
                    if (tiff_transferfunctioncount != t2p->tiff_transferfunctioncount){
                        TIFFError(
                            TIFF2PDF_MODULE,
                            "Different transfer function on page %d",
                            i);
                        t2p->t2p_error = T2P_ERR_ERROR;
                        return;
                    }
                }

                t2p->tiff_transferfunctioncount = tiff_transferfunctioncount;
                t2p->tiff_transferfunction[0] = tiff_transferfunction[0];
                t2p->tiff_transferfunction[1] = tiff_transferfunction[1];
                t2p->tiff_transferfunction[2] = tiff_transferfunction[2];
                if(tiff_transferfunctioncount == 3){
                        t2p->tiff_pages[i].page_extra += 4;
                        t2p->pdf_xrefcount += 4;
                        if(t2p->pdf_minorversion < 2)
                                t2p->pdf_minorversion = 2;
                } else if (tiff_transferfunctioncount == 1){
                        t2p->tiff_pages[i].page_extra += 2;
                        t2p->pdf_xrefcount += 2;
                        if(t2p->pdf_minorversion < 2)
                                t2p->pdf_minorversion = 2;
                }

		if( TIFFGetField(
			input, 
			TIFFTAG_ICCPROFILE, 
			&(t2p->tiff_iccprofilelength), 
			&(t2p->tiff_iccprofile)) != 0){
			t2p->tiff_pages[i].page_extra++;
			t2p->pdf_xrefcount++;
			if(t2p->pdf_minorversion<3){t2p->pdf_minorversion=3;}
		}
		t2p->tiff_tiles[i].tiles_tilecount=
			t2p->tiff_pages[i].page_tilecount;
		if( (TIFFGetField(input, TIFFTAG_PLANARCONFIG, &xuint16) != 0)
			&& (xuint16 == PLANARCONFIG_SEPARATE ) ){
				if( !TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &xuint16) )
				{
					TIFFError(
                        TIFF2PDF_MODULE, 
                        "Missing SamplesPerPixel, %s", 
                        TIFFFileName(input));
                    t2p->t2p_error = T2P_ERR_ERROR;
                    return;
				}
                if( (t2p->tiff_tiles[i].tiles_tilecount % xuint16) != 0 )
                {
                    TIFFError(
                        TIFF2PDF_MODULE, 
                        "Invalid tile count, %s", 
                        TIFFFileName(input));
                    t2p->t2p_error = T2P_ERR_ERROR;
                    return;
                }
				t2p->tiff_tiles[i].tiles_tilecount/= xuint16;
		}
		if( t2p->tiff_tiles[i].tiles_tilecount > 0){
			t2p->pdf_xrefcount += 
				(t2p->tiff_tiles[i].tiles_tilecount -1)*2;
			TIFFGetField(input, 
				TIFFTAG_TILEWIDTH, 
				&( t2p->tiff_tiles[i].tiles_tilewidth) );
			TIFFGetField(input, 
				TIFFTAG_TILELENGTH, 
				&( t2p->tiff_tiles[i].tiles_tilelength) );
			t2p->tiff_tiles[i].tiles_tiles = 
			(T2P_TILE*) _TIFFmalloc(TIFFSafeMultiply(tmsize_t,t2p->tiff_tiles[i].tiles_tilecount,
                                                                 sizeof(T2P_TILE)) );
			if( t2p->tiff_tiles[i].tiles_tiles == NULL){
				TIFFError(
					TIFF2PDF_MODULE, 
					"Can't allocate " TIFF_SIZE_FORMAT " bytes of memory for t2p_read_tiff_init, %s", 
					(TIFF_SIZE_T) t2p->tiff_tiles[i].tiles_tilecount * sizeof(T2P_TILE), 
					TIFFFileName(input));
				t2p->t2p_error = T2P_ERR_ERROR;
				return;
			}
		}
	}

	return;
}