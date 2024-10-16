const char* LoaderXM::identifyModule(const mp_ubyte* buffer)
{
	// check for .XM module first
	if (!memcmp(buffer,"Extended Module:",16))
	{
		return "XM";
	}

	// this is not an .XM
	return NULL;
}