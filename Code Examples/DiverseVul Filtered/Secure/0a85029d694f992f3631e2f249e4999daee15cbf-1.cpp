static GF_ISOSAPType sap_type_from_nal_type(u8 nal_type) {
	switch (nal_type) {
	case GF_HEVC_NALU_SLICE_CRA:
		return SAP_TYPE_3;
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
	case GF_HEVC_NALU_SLICE_BLA_N_LP:
		return SAP_TYPE_1;
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_W_LP:
		return SAP_TYPE_2;
	default:
		return RAP_NO;
	}
}