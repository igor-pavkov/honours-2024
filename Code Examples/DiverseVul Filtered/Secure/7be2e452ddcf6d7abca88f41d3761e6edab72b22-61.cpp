tsize_t t2p_write_pdf_xobject_decode(T2P* t2p, TIFF* output){

	tsize_t written=0;
	int i=0;

	written += t2pWriteFile(output, (tdata_t) "/Decode [ ", 10);
	for (i=0;i<t2p->tiff_samplesperpixel;i++){
		written += t2pWriteFile(output, (tdata_t) "1 0 ", 4);
	}
	written += t2pWriteFile(output, (tdata_t) "]\n", 2);

	return(written);
}