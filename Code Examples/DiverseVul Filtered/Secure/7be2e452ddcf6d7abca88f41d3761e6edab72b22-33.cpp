tsize_t t2p_write_pdf_transfer(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;

	written += t2pWriteFile(output, (tdata_t) "<< /Type /ExtGState \n/TR ", 25);
	if(t2p->tiff_transferfunctioncount == 1){
		buflen=snprintf(buffer, sizeof(buffer), "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 1));
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
	} else {
		written += t2pWriteFile(output, (tdata_t) "[ ", 2);
		buflen=snprintf(buffer, sizeof(buffer), "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 1));
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		buflen=snprintf(buffer, sizeof(buffer), "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 2));
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		buflen=snprintf(buffer, sizeof(buffer), "%lu",
			       (unsigned long)(t2p->pdf_xrefcount + 3));
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ", 5);
		written += t2pWriteFile(output, (tdata_t) "/Identity ] ", 12);
	}

	written += t2pWriteFile(output, (tdata_t) " >> \n", 5);

	return(written);
}