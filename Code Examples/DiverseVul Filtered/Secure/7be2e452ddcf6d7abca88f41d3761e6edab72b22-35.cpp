tsize_t t2p_write_pdf_obj_end(TIFF* output){

	tsize_t written=0;

	written += t2pWriteFile(output, (tdata_t) "endobj\n", 7);

	return(written);
}