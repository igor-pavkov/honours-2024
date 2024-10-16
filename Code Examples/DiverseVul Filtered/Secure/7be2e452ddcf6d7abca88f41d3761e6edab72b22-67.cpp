tsize_t t2p_write_pdf_transfer_stream(T2P* t2p, TIFF* output, uint16 i){

	tsize_t written=0;

	written += t2p_write_pdf_stream(
		t2p->tiff_transferfunction[i], 
		(((tsize_t)1)<<(t2p->tiff_bitspersample+1)), 
		output);

	return(written);
}