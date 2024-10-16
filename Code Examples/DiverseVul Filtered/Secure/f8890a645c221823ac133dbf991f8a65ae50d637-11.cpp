static BOOL update_set_bounds(rdpContext* context, const rdpBounds* bounds)
{
	rdpUpdate* update = context->update;
	CopyMemory(&update->previousBounds, &update->currentBounds, sizeof(rdpBounds));

	if (!bounds)
		ZeroMemory(&update->currentBounds, sizeof(rdpBounds));
	else
		CopyMemory(&update->currentBounds, bounds, sizeof(rdpBounds));

	return TRUE;
}