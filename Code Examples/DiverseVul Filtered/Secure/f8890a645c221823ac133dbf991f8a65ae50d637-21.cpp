static BOOL update_send_surface_frame_marker(rdpContext* context,
                                             const SURFACE_FRAME_MARKER* surfaceFrameMarker)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	update_force_flush(context);
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!update_write_surfcmd_frame_marker(s, surfaceFrameMarker->frameAction,
	                                       surfaceFrameMarker->frameId) ||
	    !fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_SURFCMDS, s, FALSE))
		goto out_fail;

	update_force_flush(context);
	ret = TRUE;
out_fail:
	Stream_Release(s);
	return ret;
}