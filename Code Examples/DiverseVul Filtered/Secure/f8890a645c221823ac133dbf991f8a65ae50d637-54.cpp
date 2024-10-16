static BOOL update_send_pointer_cached(rdpContext* context,
                                       const POINTER_CACHED_UPDATE* pointer_cached)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	Stream_Write_UINT16(s, pointer_cached->cacheIndex); /* cacheIndex (2 bytes) */
	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_CACHED, s, FALSE);
	Stream_Release(s);
	return ret;
}