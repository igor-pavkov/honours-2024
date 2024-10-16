static int fromiccpcs(int cs)
{
	switch (cs) {
	case ICC_CS_RGB:
		return JAS_CLRSPC_GENRGB;
	case ICC_CS_YCBCR:
		return JAS_CLRSPC_GENYCBCR;
	case ICC_CS_GRAY:
		return JAS_CLRSPC_GENGRAY;
	}
	return JAS_CLRSPC_UNKNOWN;
}