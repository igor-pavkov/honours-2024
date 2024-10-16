tsize_t t2p_write_pdf_stream_start(TIFF* output){

	tsize_t written=0;

	written += t2pWriteFile(output, (tdata_t) "stream\n", 7);

	return(written);
}