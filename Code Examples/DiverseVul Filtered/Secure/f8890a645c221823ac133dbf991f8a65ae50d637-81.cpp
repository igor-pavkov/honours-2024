rdpUpdate* update_new(rdpRdp* rdp)
{
	const wObject cb = { NULL, NULL, NULL, update_free_queued_message, NULL };
	rdpUpdate* update;
	OFFSCREEN_DELETE_LIST* deleteList;
	WINPR_UNUSED(rdp);
	update = (rdpUpdate*)calloc(1, sizeof(rdpUpdate));

	if (!update)
		return NULL;

	update->log = WLog_Get("com.freerdp.core.update");
	InitializeCriticalSection(&(update->mux));
	update->pointer = (rdpPointerUpdate*)calloc(1, sizeof(rdpPointerUpdate));

	if (!update->pointer)
		goto fail;

	update->primary = (rdpPrimaryUpdate*)calloc(1, sizeof(rdpPrimaryUpdate));

	if (!update->primary)
		goto fail;

	update->secondary = (rdpSecondaryUpdate*)calloc(1, sizeof(rdpSecondaryUpdate));

	if (!update->secondary)
		goto fail;

	update->altsec = (rdpAltSecUpdate*)calloc(1, sizeof(rdpAltSecUpdate));

	if (!update->altsec)
		goto fail;

	update->window = (rdpWindowUpdate*)calloc(1, sizeof(rdpWindowUpdate));

	if (!update->window)
		goto fail;

	deleteList = &(update->altsec->create_offscreen_bitmap.deleteList);
	deleteList->sIndices = 64;
	deleteList->indices = calloc(deleteList->sIndices, 2);

	if (!deleteList->indices)
		goto fail;

	deleteList->cIndices = 0;
	update->SuppressOutput = update_send_suppress_output;
	update->initialState = TRUE;
	update->autoCalculateBitmapData = TRUE;
	update->queue = MessageQueue_New(&cb);

	if (!update->queue)
		goto fail;

	return update;
fail:
	update_free(update);
	return NULL;
}