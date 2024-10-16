static int avi_sampsize(avi_t *AVI, int j)
{
	int s;
	s = ((AVI->track[j].a_bits+7)/8)*AVI->track[j].a_chans;
	//   if(s==0) s=1; /* avoid possible zero divisions */
	if(s<4) s=4; /* avoid possible zero divisions */
	return s;
}