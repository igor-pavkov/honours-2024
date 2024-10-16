tsize_t t2p_write_pdf_page(uint32 object, T2P* t2p, TIFF* output){

	unsigned int i=0;
	tsize_t written=0;
	char buffer[256];
	int buflen=0;

	written += t2pWriteFile(output, (tdata_t) "<<\n/Type /Page \n/Parent ", 24);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_pages);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " 0 R \n", 6);
	written += t2pWriteFile(output, (tdata_t) "/MediaBox [", 11); 
	buflen=snprintf(buffer, sizeof(buffer), "%.4f",t2p->pdf_mediabox.x1);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " ", 1); 
	buflen=snprintf(buffer, sizeof(buffer), "%.4f",t2p->pdf_mediabox.y1);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " ", 1); 
	buflen=snprintf(buffer, sizeof(buffer), "%.4f",t2p->pdf_mediabox.x2);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " ", 1); 
	buflen=snprintf(buffer, sizeof(buffer), "%.4f",t2p->pdf_mediabox.y2);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "] \n", 3); 
	written += t2pWriteFile(output, (tdata_t) "/Contents ", 10);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)(object + 1));
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " 0 R \n", 6);
	written += t2pWriteFile(output, (tdata_t) "/Resources << \n", 15);
	if( t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount != 0 ){
		written += t2pWriteFile(output, (tdata_t) "/XObject <<\n", 12);
		for(i=0;i<t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount;i++){
			written += t2pWriteFile(output, (tdata_t) "/Im", 3);
			buflen = snprintf(buffer, sizeof(buffer), "%u", t2p->pdf_page+1);
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) "_", 1);
			buflen = snprintf(buffer, sizeof(buffer), "%u", i+1);
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) " ", 1);
			buflen = snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)(object+3+(2*i)+t2p->tiff_pages[t2p->pdf_page].page_extra)); 
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
			if(i%4==3){
				written += t2pWriteFile(output, (tdata_t) "\n", 1);
			}
		}
		written += t2pWriteFile(output, (tdata_t) ">>\n", 3);
	} else {
			written += t2pWriteFile(output, (tdata_t) "/XObject <<\n", 12);
			written += t2pWriteFile(output, (tdata_t) "/Im", 3);
			buflen = snprintf(buffer, sizeof(buffer), "%u", t2p->pdf_page+1);
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) " ", 1);
			buflen = snprintf(buffer, sizeof(buffer), "%lu",
				(unsigned long)(object+3+(2*i)+t2p->tiff_pages[t2p->pdf_page].page_extra)); 
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		written += t2pWriteFile(output, (tdata_t) ">>\n", 3);
	}
	if(t2p->tiff_transferfunctioncount != 0) {
		written += t2pWriteFile(output, (tdata_t) "/ExtGState <<", 13);
		t2pWriteFile(output, (tdata_t) "/GS1 ", 5);
		buflen = snprintf(buffer, sizeof(buffer), "%lu",
			(unsigned long)(object + 3)); 
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		written += t2pWriteFile(output, (tdata_t) ">> \n", 4);
	}
	written += t2pWriteFile(output, (tdata_t) "/ProcSet [ ", 11);
	if(t2p->pdf_colorspace & T2P_CS_BILEVEL 
		|| t2p->pdf_colorspace & T2P_CS_GRAY
		){
		written += t2pWriteFile(output, (tdata_t) "/ImageB ", 8);
	} else {
		written += t2pWriteFile(output, (tdata_t) "/ImageC ", 8);
		if(t2p->pdf_colorspace & T2P_CS_PALETTE){
			written += t2pWriteFile(output, (tdata_t) "/ImageI ", 8);
		}
	}
	written += t2pWriteFile(output, (tdata_t) "]\n>>\n>>\n", 8);

	return(written);
}