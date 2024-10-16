struct bgp_notify bgp_notify_decapsulate_hard_reset(struct bgp_notify *notify)
{
	struct bgp_notify bn = {};

	bn.code = notify->raw_data[0];
	bn.subcode = notify->raw_data[1];
	bn.length = notify->length - 2;

	bn.raw_data = XMALLOC(MTYPE_BGP_NOTIFICATION, bn.length);
	memcpy(bn.raw_data, notify->raw_data + 2, bn.length);

	return bn;
}