tsize_t t2p_sample_realize_palette(T2P* t2p, unsigned char* buffer){

	uint32 sample_count=0;
	uint16 component_count=0;
	uint32 palette_offset=0;
	uint32 sample_offset=0;
	uint32 i=0;
	uint32 j=0;
	sample_count=t2p->tiff_width*t2p->tiff_length;
	component_count=t2p->tiff_samplesperpixel;
        if( sample_count * component_count > t2p->tiff_datasize )
        {
            TIFFError(TIFF2PDF_MODULE,  "Error: sample_count * component_count > t2p->tiff_datasize");
            t2p->t2p_error = T2P_ERR_ERROR;
            return 1;
        }
	
	for(i=sample_count;i>0;i--){
		palette_offset=buffer[i-1] * component_count;
		sample_offset= (i-1) * component_count;
		for(j=0;j<component_count;j++){
			buffer[sample_offset+j]=t2p->pdf_palette[palette_offset+j];
		}
	}

	return(0);
}