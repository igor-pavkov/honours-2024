const char* LoaderS3M::identifyModule(const mp_ubyte* buffer)
{
	// check for .S3M module
	if (!memcmp(buffer+0x2C,"SCRM",4)) 
	{
		return "S3M";
	}

	// this is not an .S3M
	return NULL;
}