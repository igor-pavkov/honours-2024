tsize_t t2p_write_pdf_stream_dict_end(TIFF* output){

	tsize_t written=0;

	written += t2pWriteFile(output, (tdata_t) " >>\n", 4);

	return(written);
}