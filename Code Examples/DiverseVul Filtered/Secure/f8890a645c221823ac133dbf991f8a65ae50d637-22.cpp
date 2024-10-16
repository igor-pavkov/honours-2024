static int update_prepare_bounds(rdpContext* context, ORDER_INFO* orderInfo)
{
	int length = 0;
	rdpUpdate* update = context->update;
	orderInfo->boundsFlags = 0;

	if (update_bounds_is_null(&update->currentBounds))
		return 0;

	orderInfo->controlFlags |= ORDER_BOUNDS;

	if (update_bounds_equals(&update->previousBounds, &update->currentBounds))
	{
		orderInfo->controlFlags |= ORDER_ZERO_BOUNDS_DELTAS;
		return 0;
	}
	else
	{
		length += 1;

		if (update->previousBounds.left != update->currentBounds.left)
		{
			orderInfo->bounds.left = update->currentBounds.left;
			orderInfo->boundsFlags |= BOUND_LEFT;
			length += 2;
		}

		if (update->previousBounds.top != update->currentBounds.top)
		{
			orderInfo->bounds.top = update->currentBounds.top;
			orderInfo->boundsFlags |= BOUND_TOP;
			length += 2;
		}

		if (update->previousBounds.right != update->currentBounds.right)
		{
			orderInfo->bounds.right = update->currentBounds.right;
			orderInfo->boundsFlags |= BOUND_RIGHT;
			length += 2;
		}

		if (update->previousBounds.bottom != update->currentBounds.bottom)
		{
			orderInfo->bounds.bottom = update->currentBounds.bottom;
			orderInfo->boundsFlags |= BOUND_BOTTOM;
			length += 2;
		}
	}

	return length;
}