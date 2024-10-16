tsize_t t2p_sample_planar_separate_to_contig(
											T2P* t2p, 
											unsigned char* buffer, 
											unsigned char* samplebuffer, 
											tsize_t samplebuffersize){

	tsize_t stride=0;
	tsize_t i=0;
	tsize_t j=0;
	
	stride=samplebuffersize/t2p->tiff_samplesperpixel;
	for(i=0;i<stride;i++){
		for(j=0;j<t2p->tiff_samplesperpixel;j++){
			buffer[i*t2p->tiff_samplesperpixel + j] = samplebuffer[i + j*stride];
		}
	}

	return(samplebuffersize);
}