tsize_t t2p_write_pdf_xobject_palettecs_stream(T2P* t2p, TIFF* output){

	tsize_t written=0;

	written += t2p_write_pdf_stream(
				(tdata_t) t2p->pdf_palette, 
				(tsize_t) t2p->pdf_palettesize, 
				output);
	
	return(written);
}