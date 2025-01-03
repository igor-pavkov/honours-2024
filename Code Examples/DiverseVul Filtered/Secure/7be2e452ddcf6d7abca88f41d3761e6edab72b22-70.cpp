tsize_t t2p_write_pdf_stream_length(tsize_t len, TIFF* output){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;

	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)len);
	check_snprintf_ret((T2P*)NULL, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n", 1);

	return(written);
}