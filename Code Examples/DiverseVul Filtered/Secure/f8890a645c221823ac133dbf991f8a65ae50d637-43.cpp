static BOOL update_send_bitmap_update(rdpContext* context, const BITMAP_UPDATE* bitmapUpdate)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	rdpUpdate* update = context->update;
	BOOL ret = TRUE;
	update_force_flush(context);
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!update_write_bitmap_update(update, s, bitmapUpdate) ||
	    !fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_BITMAP, s,
	                              bitmapUpdate->skipCompression))
	{
		ret = FALSE;
		goto out_fail;
	}

	update_force_flush(context);
out_fail:
	Stream_Release(s);
	return ret;
}