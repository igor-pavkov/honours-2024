static void lsr_read_private_attribute_container(GF_LASeRCodec *lsr)
{
	u32 val;
	do {
		u32 skip_len;
		GF_LSR_READ_INT(lsr, val, 2, "privateDataType");
		skip_len = lsr_read_vluimsbf5(lsr, "skipLen");
		gf_bs_align(lsr->bs);
		/*just skip data*/
#if 1
		if (skip_len>gf_bs_available(lsr->bs)) {
			lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;
			return;
		}
		gf_bs_skip_bytes(lsr->bs, skip_len);
#else
		switch (val) {
		/*private data of type "anyXML"*/
		case 0:
			count = lsr_read_vluimsbf5(lsr, "count");
			for (i=0; i<count; i++) {
				privateAttribute(0) attr[i];
			}
			break;
		case 1:
			/*TODO FIXME - nameSpaceIndexBits is not defined in the spec*/
			uint(nameSpaceIndexBits) nameSpaceIndex;
			gf_bs_align(lsr->bs);
			byte[skipLen - ((nameSpaceIndexBits+7)%8)] data;
			break;
		default:
			/*TODO - spec is wrong here (typo, "len" instead of "skipLen" )*/
			gf_bs_skip_bytes(skipLen);
			break;
		}
#endif
		gf_bs_align(lsr->bs);
		GF_LSR_READ_INT(lsr, val, 1, "hasMorePrivateData");
	} while (val);
}