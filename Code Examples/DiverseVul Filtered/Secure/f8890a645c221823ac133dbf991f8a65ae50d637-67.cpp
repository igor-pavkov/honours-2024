static BOOL update_send_pointer_color(rdpContext* context,
                                      const POINTER_COLOR_UPDATE* pointer_color)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!update_write_pointer_color(s, pointer_color))
		goto out_fail;

	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_COLOR, s, FALSE);
out_fail:
	Stream_Release(s);
	return ret;
}