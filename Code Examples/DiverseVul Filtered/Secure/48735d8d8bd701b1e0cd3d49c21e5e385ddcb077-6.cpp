static int lbs_scan_add_chanlist_tlv(uint8_t *tlv,
				     struct chanscanparamset *chan_list,
				     int chan_count)
{
	size_t size = sizeof(struct chanscanparamset) *chan_count;
	struct mrvlietypes_chanlistparamset *chan_tlv = (void *)tlv;

	chan_tlv->header.type = cpu_to_le16(TLV_TYPE_CHANLIST);
	memcpy(chan_tlv->chanscanparam, chan_list, size);
	chan_tlv->header.len = cpu_to_le16(size);
	return sizeof(chan_tlv->header) + size;
}