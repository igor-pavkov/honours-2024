tsize_t t2p_write_pdf_pages(T2P* t2p, TIFF* output)
{
	tsize_t written=0;
	tdir_t i=0;
	char buffer[32];
	int buflen=0;

	int page=0;
	written += t2pWriteFile(output,
		(tdata_t) "<< \n/Type /Pages \n/Kids [ ", 26);
	page = t2p->pdf_pages+1;
	for (i=0;i<t2p->tiff_pagecount;i++){
		buflen=snprintf(buffer, sizeof(buffer), "%d", page);
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		if ( ((i+1)%8)==0 ) {
			written += t2pWriteFile(output, (tdata_t) "\n", 1);
		}
		page +=3;
		page += t2p->tiff_pages[i].page_extra;
		if(t2p->tiff_pages[i].page_tilecount>0){
			page += (2 * t2p->tiff_pages[i].page_tilecount);
		} else {
			page +=2;
		}
	}
	written += t2pWriteFile(output, (tdata_t) "] \n/Count ", 10);
	buflen=snprintf(buffer, sizeof(buffer), "%d", t2p->tiff_pagecount);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " \n>> \n", 6);

	return(written);
}