tsize_t t2p_write_pdf_xobject_icccs(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;
	
	written += t2pWriteFile(output, (tdata_t) "[/ICCBased ", 11);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_icccs);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " 0 R] \n", 7);

	return(written);
}