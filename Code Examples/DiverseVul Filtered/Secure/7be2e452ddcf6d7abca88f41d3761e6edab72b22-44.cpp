tsize_t t2p_sample_abgr_to_rgb(tdata_t data, uint32 samplecount)
{
	uint32 i=0;
	uint32 sample=0;
	
	for(i=0;i<samplecount;i++){
		sample=((uint32*)data)[i];
		((char*)data)[i*3]= (char) (sample & 0xff);
		((char*)data)[i*3+1]= (char) ((sample>>8) & 0xff);
		((char*)data)[i*3+2]= (char) ((sample>>16) & 0xff);
	}

	return(i*3);
}