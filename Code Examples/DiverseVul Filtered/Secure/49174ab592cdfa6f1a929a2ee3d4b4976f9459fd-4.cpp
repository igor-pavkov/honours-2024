static int jp2_getcs(jp2_colr_t *colr)
{
	if (colr->method == JP2_COLR_ENUM) {
		switch (colr->csid) {
		case JP2_COLR_SRGB:
			return JAS_CLRSPC_SRGB;
		case JP2_COLR_SYCC:
			return JAS_CLRSPC_SYCBCR;
		case JP2_COLR_SGRAY:
			return JAS_CLRSPC_SGRAY;
		}
	}
	return JAS_CLRSPC_UNKNOWN;
}