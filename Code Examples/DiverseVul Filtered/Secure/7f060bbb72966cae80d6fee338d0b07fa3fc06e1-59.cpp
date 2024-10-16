int AVI_read_data(avi_t *AVI, char *vidbuf, int max_vidbuf,
                  char *audbuf, int max_audbuf,
                  int *len)
{

	/*
	 * Return codes:
	 *
	 *    1 = video data read
	 *    2 = audio data read
	 *    0 = reached EOF
	 *   -1 = video buffer too small
	 *   -2 = audio buffer too small
	 */

	s64 n;
	char data[8];

	if(AVI->mode==AVI_MODE_WRITE) return 0;

	while(1)
	{
		/* Read tag and length */

		if( avi_read(AVI->fdes,data,8) != 8 ) return 0;

		/* if we got a list tag, ignore it */

		if(strnicmp(data,"LIST",4) == 0)
		{
			gf_fseek(AVI->fdes,4,SEEK_CUR);
			continue;
		}

		n = PAD_EVEN(str2ulong((unsigned char *)data+4));

		if(strnicmp(data,AVI->video_tag,3) == 0)
		{
			*len = (u32) n;
			AVI->video_pos++;
			if(n>max_vidbuf)
			{
				gf_fseek(AVI->fdes,n,SEEK_CUR);
				return -1;
			}
			if(avi_read(AVI->fdes,vidbuf, (u32) n) != n ) return 0;
			return 1;
		}
		else if(strnicmp(data,AVI->track[AVI->aptr].audio_tag,4) == 0)
		{
			*len = (u32) n;
			if(n>max_audbuf)
			{
				gf_fseek(AVI->fdes,n,SEEK_CUR);
				return -2;
			}
			if(avi_read(AVI->fdes,audbuf, (u32) n) != n ) return 0;
			return 2;
			break;
		}
		else if(gf_fseek(AVI->fdes,n,SEEK_CUR) == (u64) -1)  return 0;
	}
}