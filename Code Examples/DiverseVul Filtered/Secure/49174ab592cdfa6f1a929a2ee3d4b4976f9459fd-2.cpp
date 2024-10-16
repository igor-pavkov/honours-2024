static int jp2_getct(int colorspace, int type, int assoc)
{
	if (type == 1 && assoc == 0) {
		return JAS_IMAGE_CT_OPACITY;
	}
	if (type == 0 && assoc >= 1 && assoc <= 65534) {
		switch (colorspace) {
		case JAS_CLRSPC_FAM_RGB:
			switch (assoc) {
			case JP2_CDEF_RGB_R:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R);
			case JP2_CDEF_RGB_G:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G);
			case JP2_CDEF_RGB_B:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B);
			}
			break;
		case JAS_CLRSPC_FAM_YCBCR:
			switch (assoc) {
			case JP2_CDEF_YCBCR_Y:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_YCBCR_Y);
			case JP2_CDEF_YCBCR_CB:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_YCBCR_CB);
			case JP2_CDEF_YCBCR_CR:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_YCBCR_CR);
			}
			break;
		case JAS_CLRSPC_FAM_GRAY:
			switch (assoc) {
			case JP2_CDEF_GRAY_Y:
				return JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y);
			}
			break;
		default:
			return JAS_IMAGE_CT_COLOR(assoc - 1);
		}
	}
	return JAS_IMAGE_CT_UNKNOWN;
}