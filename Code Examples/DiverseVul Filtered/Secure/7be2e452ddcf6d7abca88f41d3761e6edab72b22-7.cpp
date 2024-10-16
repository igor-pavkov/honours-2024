tsize_t t2p_write_pdf_stream(tdata_t buffer, tsize_t len, TIFF* output){

	tsize_t written=0;

	written += t2pWriteFile(output, (tdata_t) buffer, len);

	return(written);
}