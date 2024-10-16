tsize_t t2p_write_pdf_xobject_icccs_stream(T2P* t2p, TIFF* output){

	tsize_t written=0;

	written += t2p_write_pdf_stream(
				(tdata_t) t2p->tiff_iccprofile, 
				(tsize_t) t2p->tiff_iccprofilelength, 
				output);
	
	return(written);
}