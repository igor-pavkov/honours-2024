static BOOL update_send_synchronize(rdpContext* context)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	Stream_Zero(s, 2); /* pad2Octets (2 bytes) */
	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_SYNCHRONIZE, s, FALSE);
	Stream_Release(s);
	return ret;
}