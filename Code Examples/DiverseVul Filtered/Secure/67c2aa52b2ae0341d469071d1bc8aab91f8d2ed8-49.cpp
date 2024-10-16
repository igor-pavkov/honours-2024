BOOL update_write_bounds(wStream* s, ORDER_INFO* orderInfo)
{
	if (!(orderInfo->controlFlags & ORDER_BOUNDS))
		return TRUE;

	if (orderInfo->controlFlags & ORDER_ZERO_BOUNDS_DELTAS)
		return TRUE;

	Stream_Write_UINT8(s, orderInfo->boundsFlags); /* field flags */

	if (orderInfo->boundsFlags & BOUND_LEFT)
	{
		if (!update_write_coord(s, orderInfo->bounds.left))
			return FALSE;
	}
	else if (orderInfo->boundsFlags & BOUND_DELTA_LEFT)
	{
	}

	if (orderInfo->boundsFlags & BOUND_TOP)
	{
		if (!update_write_coord(s, orderInfo->bounds.top))
			return FALSE;
	}
	else if (orderInfo->boundsFlags & BOUND_DELTA_TOP)
	{
	}

	if (orderInfo->boundsFlags & BOUND_RIGHT)
	{
		if (!update_write_coord(s, orderInfo->bounds.right))
			return FALSE;
	}
	else if (orderInfo->boundsFlags & BOUND_DELTA_RIGHT)
	{
	}

	if (orderInfo->boundsFlags & BOUND_BOTTOM)
	{
		if (!update_write_coord(s, orderInfo->bounds.bottom))
			return FALSE;
	}
	else if (orderInfo->boundsFlags & BOUND_DELTA_BOTTOM)
	{
	}

	return TRUE;
}