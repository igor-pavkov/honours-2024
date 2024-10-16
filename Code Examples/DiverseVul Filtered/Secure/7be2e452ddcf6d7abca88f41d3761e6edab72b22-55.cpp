tsize_t t2p_write_pdf_page_content_stream(T2P* t2p, TIFF* output){

	tsize_t written=0;
	ttile_t i=0;
	char buffer[512];
	int buflen=0;
	T2P_BOX box;
	
	if(t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount>0){ 
		for(i=0;i<t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount; i++){
			box=t2p->tiff_tiles[t2p->pdf_page].tiles_tiles[i].tile_box;
			buflen=snprintf(buffer, sizeof(buffer), 
				"q %s %.4f %.4f %.4f %.4f %.4f %.4f cm /Im%d_%ld Do Q\n", 
				t2p->tiff_transferfunctioncount?"/GS1 gs ":"",
				box.mat[0],
				box.mat[1],
				box.mat[3],
				box.mat[4],
				box.mat[6],
				box.mat[7],
				t2p->pdf_page + 1, 
				(long)(i + 1));
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2p_write_pdf_stream(buffer, buflen, output);
		}
	} else {
		box=t2p->pdf_imagebox;
		buflen=snprintf(buffer, sizeof(buffer), 
			"q %s %.4f %.4f %.4f %.4f %.4f %.4f cm /Im%d Do Q\n", 
			t2p->tiff_transferfunctioncount?"/GS1 gs ":"",
			box.mat[0],
			box.mat[1],
			box.mat[3],
			box.mat[4],
			box.mat[6],
			box.mat[7],
			t2p->pdf_page+1);
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2p_write_pdf_stream(buffer, buflen, output);
	}

	return(written);
}