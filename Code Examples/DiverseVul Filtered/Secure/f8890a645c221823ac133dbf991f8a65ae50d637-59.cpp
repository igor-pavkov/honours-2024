BOOL update_recv_pointer(rdpUpdate* update, wStream* s)
{
	BOOL rc = FALSE;
	UINT16 messageType;
	rdpContext* context = update->context;
	rdpPointerUpdate* pointer = update->pointer;

	if (Stream_GetRemainingLength(s) < 2 + 2)
		return FALSE;

	Stream_Read_UINT16(s, messageType); /* messageType (2 bytes) */
	Stream_Seek_UINT16(s);              /* pad2Octets (2 bytes) */

	switch (messageType)
	{
		case PTR_MSG_TYPE_POSITION:
		{
			POINTER_POSITION_UPDATE* pointer_position = update_read_pointer_position(update, s);

			if (pointer_position)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerPosition, context, pointer_position);
				free_pointer_position_update(context, pointer_position);
			}
		}
		break;

		case PTR_MSG_TYPE_SYSTEM:
		{
			POINTER_SYSTEM_UPDATE* pointer_system = update_read_pointer_system(update, s);

			if (pointer_system)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerSystem, context, pointer_system);
				free_pointer_system_update(context, pointer_system);
			}
		}
		break;

		case PTR_MSG_TYPE_COLOR:
		{
			POINTER_COLOR_UPDATE* pointer_color = update_read_pointer_color(update, s, 24);

			if (pointer_color)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerColor, context, pointer_color);
				free_pointer_color_update(context, pointer_color);
			}
		}
		break;

		case PTR_MSG_TYPE_POINTER_LARGE:
		{
			POINTER_LARGE_UPDATE* pointer_large = update_read_pointer_large(update, s);

			if (pointer_large)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerLarge, context, pointer_large);
				free_pointer_large_update(context, pointer_large);
			}
		}
		break;

		case PTR_MSG_TYPE_POINTER:
		{
			POINTER_NEW_UPDATE* pointer_new = update_read_pointer_new(update, s);

			if (pointer_new)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerNew, context, pointer_new);
				free_pointer_new_update(context, pointer_new);
			}
		}
		break;

		case PTR_MSG_TYPE_CACHED:
		{
			POINTER_CACHED_UPDATE* pointer_cached = update_read_pointer_cached(update, s);

			if (pointer_cached)
			{
				rc = IFCALLRESULT(FALSE, pointer->PointerCached, context, pointer_cached);
				free_pointer_cached_update(context, pointer_cached);
			}
		}
		break;

		default:
			break;
	}

	return rc;
}