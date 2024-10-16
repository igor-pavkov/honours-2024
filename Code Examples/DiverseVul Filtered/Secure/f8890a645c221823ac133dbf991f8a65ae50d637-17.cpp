BOOL update_end_paint(rdpUpdate* update)
{
	BOOL rc = FALSE;

	if (!update)
		return FALSE;

	if (update->EndPaint)
		rc = update->EndPaint(update->context);

	LeaveCriticalSection(&update->mux);
	return rc;
}