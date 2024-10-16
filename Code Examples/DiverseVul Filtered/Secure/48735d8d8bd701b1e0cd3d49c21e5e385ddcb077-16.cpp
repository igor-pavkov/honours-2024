static int lbs_scan_add_rates_tlv(uint8_t *tlv)
{
	int i;
	struct mrvlietypes_ratesparamset *rate_tlv = (void *)tlv;

	rate_tlv->header.type = cpu_to_le16(TLV_TYPE_RATES);
	tlv += sizeof(rate_tlv->header);
	for (i = 0; i < MAX_RATES; i++) {
		*tlv = lbs_bg_rates[i];
		if (*tlv == 0)
			break;
		/* This code makes sure that the 802.11b rates (1 MBit/s, 2
		   MBit/s, 5.5 MBit/s and 11 MBit/s get's the high bit set.
		   Note that the values are MBit/s * 2, to mark them as
		   basic rates so that the firmware likes it better */
		if (*tlv == 0x02 || *tlv == 0x04 ||
		    *tlv == 0x0b || *tlv == 0x16)
			*tlv |= 0x80;
		tlv++;
	}
	rate_tlv->header.len = cpu_to_le16(i);
	return sizeof(rate_tlv->header) + i;
}