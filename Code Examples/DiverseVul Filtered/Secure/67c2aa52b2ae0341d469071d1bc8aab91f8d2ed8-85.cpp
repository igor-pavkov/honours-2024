int update_approximate_create_offscreen_bitmap_order(
    const CREATE_OFFSCREEN_BITMAP_ORDER* create_offscreen_bitmap)
{
	const OFFSCREEN_DELETE_LIST* deleteList = &(create_offscreen_bitmap->deleteList);
	return 32 + deleteList->cIndices * 2;
}