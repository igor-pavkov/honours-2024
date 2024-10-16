BOOL update_write_create_offscreen_bitmap_order(
    wStream* s, const CREATE_OFFSCREEN_BITMAP_ORDER* create_offscreen_bitmap)
{
	UINT16 flags;
	BOOL deleteListPresent;
	const OFFSCREEN_DELETE_LIST* deleteList;

	if (!Stream_EnsureRemainingCapacity(
	        s, update_approximate_create_offscreen_bitmap_order(create_offscreen_bitmap)))
		return FALSE;

	deleteList = &(create_offscreen_bitmap->deleteList);
	flags = create_offscreen_bitmap->id & 0x7FFF;
	deleteListPresent = (deleteList->cIndices > 0) ? TRUE : FALSE;

	if (deleteListPresent)
		flags |= 0x8000;

	Stream_Write_UINT16(s, flags);                       /* flags (2 bytes) */
	Stream_Write_UINT16(s, create_offscreen_bitmap->cx); /* cx (2 bytes) */
	Stream_Write_UINT16(s, create_offscreen_bitmap->cy); /* cy (2 bytes) */

	if (deleteListPresent)
	{
		int i;
		Stream_Write_UINT16(s, deleteList->cIndices);

		for (i = 0; i < (int)deleteList->cIndices; i++)
		{
			Stream_Write_UINT16(s, deleteList->indices[i]);
		}
	}

	return TRUE;
}