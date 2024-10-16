tsize_t t2p_write_pdf_xreftable(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[64];
	int buflen=0;
	uint32 i=0;

	written += t2pWriteFile(output, (tdata_t) "xref\n0 ", 7);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)(t2p->pdf_xrefcount + 1));
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " \n0000000000 65535 f \n", 22);
	for (i=0;i<t2p->pdf_xrefcount;i++){
		snprintf(buffer, sizeof(buffer), "%.10lu 00000 n \n",
			(unsigned long)t2p->pdf_xrefoffsets[i]);
		written += t2pWriteFile(output, (tdata_t) buffer, 20);
	}

	return(written);
}