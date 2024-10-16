std::istream *istream_file(const std::string &fname)
{
	LOG_FS << "Streaming " << fname << " for reading.\n";
	if (fname.empty())
	{
		ERR_FS << "Trying to open file with empty name.\n";
		std::ifstream *s = new std::ifstream();
		s->clear(std::ios_base::failbit);
		return s;
	}
#ifndef _WIN32
	// TODO: Should also be done for Windows; but *nix systems should
	// already be sufficient to catch most offenders.
	if (fname[0] != '/') {
		WRN_FS << "Trying to open file with relative path: '" << fname << "'.\n";
#if 0
		std::ifstream *s = new std::ifstream();
		s->clear(std::ios_base::failbit);
		return s;
#endif
	}
#endif
	std::ifstream *s = new std::ifstream(fname.c_str(),std::ios_base::binary);
	if (s->is_open())
		return s;
	ERR_FS << "Could not open '" << fname << "' for reading.\n";
	return s;

}