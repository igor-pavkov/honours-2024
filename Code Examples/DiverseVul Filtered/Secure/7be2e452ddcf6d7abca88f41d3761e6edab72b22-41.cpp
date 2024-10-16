tsize_t t2p_sample_lab_signed_to_unsigned(tdata_t buffer, uint32 samplecount){

	uint32 i=0;

	for(i=0;i<samplecount;i++){
		if( (((unsigned char*)buffer)[(i*3)+1] & 0x80) !=0){
			((unsigned char*)buffer)[(i*3)+1] =
				(unsigned char)(0x80 + ((char*)buffer)[(i*3)+1]);
		} else {
			((unsigned char*)buffer)[(i*3)+1] |= 0x80;
		}
		if( (((unsigned char*)buffer)[(i*3)+2] & 0x80) !=0){
			((unsigned char*)buffer)[(i*3)+2] =
				(unsigned char)(0x80 + ((char*)buffer)[(i*3)+2]);
		} else {
			((unsigned char*)buffer)[(i*3)+2] |= 0x80;
		}
	}

	return(samplecount*3);
}