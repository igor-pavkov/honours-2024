tsize_t t2p_write_pdf_stream_end(TIFF* output){

	tsize_t written=0;

	written += t2pWriteFile(output, (tdata_t) "\nendstream\n", 11);

	return(written);
}