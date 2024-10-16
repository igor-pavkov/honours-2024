static int _gd2ReadChunk (int offset, char *compBuf, int compSize, char *chunkBuf, uLongf * chunkLen, gdIOCtx * in)
{
	int zerr;

	if (gdTell(in) != offset) {
		GD2_DBG(php_gd_error("Positioning in file to %d", offset));
		gdSeek(in, offset);
	} else {
		GD2_DBG(php_gd_error("Already Positioned in file to %d", offset));
	}

	/* Read and uncompress an entire chunk. */
	GD2_DBG(php_gd_error("Reading file"));
	if (gdGetBuf(compBuf, compSize, in) != compSize) {
		return FALSE;
	}
	GD2_DBG(php_gd_error("Got %d bytes. Uncompressing into buffer of %d bytes", compSize, (int)*chunkLen));
	zerr = uncompress((unsigned char *) chunkBuf, chunkLen, (unsigned char *) compBuf, compSize);
	if (zerr != Z_OK) {
		GD2_DBG(php_gd_error("Error %d from uncompress", zerr));
		return FALSE;
	}
	GD2_DBG(php_gd_error("Got chunk"));

	return TRUE;
}