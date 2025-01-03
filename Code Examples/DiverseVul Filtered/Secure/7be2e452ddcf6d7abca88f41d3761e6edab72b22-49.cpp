tsize_t t2p_write_pdf_stream_dict(tsize_t len, uint32 number, TIFF* output){
	
	tsize_t written=0;
	char buffer[32];
	int buflen=0;
	
	written += t2pWriteFile(output, (tdata_t) "/Length ", 8);
	if(len!=0){
		written += t2p_write_pdf_stream_length(len, output);
	} else {
		buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)number);
		check_snprintf_ret((T2P*)NULL, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R \n", 6);
	}
	
	return(written);
}