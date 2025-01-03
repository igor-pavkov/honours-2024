tsize_t t2p_write_pdf_xobject_icccs_dict(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[32];
	int buflen=0;
	
	written += t2pWriteFile(output, (tdata_t) "/N ", 3);
	buflen=snprintf(buffer, sizeof(buffer), "%u \n", t2p->tiff_samplesperpixel);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "/Alternate ", 11);
	t2p->pdf_colorspace ^= T2P_CS_ICCBASED;
	written += t2p_write_pdf_xobject_cs(t2p, output);
	t2p->pdf_colorspace |= T2P_CS_ICCBASED;
	written += t2p_write_pdf_stream_dict(t2p->tiff_iccprofilelength, 0, output);
	
	return(written);
}