static BOOL update_send_pointer_new(rdpContext* context, const POINTER_NEW_UPDATE* pointer_new)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, 16))
		goto out_fail;

	Stream_Write_UINT16(s, pointer_new->xorBpp); /* xorBpp (2 bytes) */
	update_write_pointer_color(s, &pointer_new->colorPtrAttr);
	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_POINTER, s, FALSE);
out_fail:
	Stream_Release(s);
	return ret;
}