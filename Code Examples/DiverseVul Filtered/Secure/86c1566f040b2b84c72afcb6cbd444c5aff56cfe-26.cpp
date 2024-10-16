GF_Err AdjustHintInfo(GF_HintSampleEntryBox *entry, u32 HintSampleNumber)
{
	u32 offset, count, i, size;
	GF_Err e;

	offset = gf_isom_hint_sample_size(entry->hint_sample) - entry->hint_sample->dataLength;
	count = gf_list_count(entry->hint_sample->packetTable);

	for (i=0; i<count; i++) {
		GF_HintPacket *pck = (GF_HintPacket *)gf_list_get(entry->hint_sample->packetTable, i);
		if (offset && entry->hint_sample->dataLength) {
			//adjust any offset in this packet
			e = gf_isom_hint_pck_offset(pck, offset, HintSampleNumber);
			if (e) return e;
		}
		//adjust the max packet size for this sample entry...
		size = gf_isom_hint_pck_length(pck);
		if (entry->MaxPacketSize < size) entry->MaxPacketSize = size;
	}
	return GF_OK;
}