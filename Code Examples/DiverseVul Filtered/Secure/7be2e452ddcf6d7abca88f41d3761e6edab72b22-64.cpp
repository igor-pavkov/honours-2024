tsize_t t2p_write_pdf_xobject_stream_dict(ttile_t tile, 
												T2P* t2p, 
												TIFF* output){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;

	written += t2p_write_pdf_stream_dict(0, t2p->pdf_xrefcount+1, output); 
	written += t2pWriteFile(output, 
		(tdata_t) "/Type /XObject \n/Subtype /Image \n/Name /Im", 
		42);
	buflen=snprintf(buffer, sizeof(buffer), "%u", t2p->pdf_page+1);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	if(tile != 0){
		written += t2pWriteFile(output, (tdata_t) "_", 1);
		buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)tile);
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	}
	written += t2pWriteFile(output, (tdata_t) "\n/Width ", 8);
	if(tile==0){
		buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->tiff_width);
	} else {
		if(t2p_tile_is_right_edge(t2p->tiff_tiles[t2p->pdf_page], tile-1)!=0){
			buflen=snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)t2p->tiff_tiles[t2p->pdf_page].tiles_edgetilewidth);
		} else {
			buflen=snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)t2p->tiff_tiles[t2p->pdf_page].tiles_tilewidth);
		}
	}
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n/Height ", 9);
	if(tile==0){
		buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->tiff_length);
	} else {
		if(t2p_tile_is_bottom_edge(t2p->tiff_tiles[t2p->pdf_page], tile-1)!=0){
			buflen=snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)t2p->tiff_tiles[t2p->pdf_page].tiles_edgetilelength);
		} else {
			buflen=snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)t2p->tiff_tiles[t2p->pdf_page].tiles_tilelength);
		}
	}
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n/BitsPerComponent ", 19);
	buflen=snprintf(buffer, sizeof(buffer), "%u", t2p->tiff_bitspersample);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n/ColorSpace ", 13);
	written += t2p_write_pdf_xobject_cs(t2p, output);
	if (t2p->pdf_image_interpolate)
		written += t2pWriteFile(output,
					 (tdata_t) "\n/Interpolate true", 18);
	if( (t2p->pdf_switchdecode != 0)
#ifdef CCITT_SUPPORT
		&& ! (t2p->pdf_colorspace & T2P_CS_BILEVEL 
		&& t2p->pdf_compression == T2P_COMPRESS_G4)
#endif
		){
		written += t2p_write_pdf_xobject_decode(t2p, output);
	}
	written += t2p_write_pdf_xobject_stream_filter(tile, t2p, output);
	
	return(written);
}