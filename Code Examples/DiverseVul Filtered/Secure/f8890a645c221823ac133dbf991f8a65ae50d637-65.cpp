static BOOL update_send_pointer_large(rdpContext* context, const POINTER_LARGE_UPDATE* pointer)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!update_write_pointer_large(s, pointer))
		goto out_fail;

	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_LARGE_POINTER, s, FALSE);
out_fail:
	Stream_Release(s);
	return ret;
}