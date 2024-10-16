static BOOL update_write_pointer_large(wStream* s, const POINTER_LARGE_UPDATE* pointer)
{
	if (!Stream_EnsureRemainingCapacity(s, 32 + pointer->lengthAndMask + pointer->lengthXorMask))
		return FALSE;

	Stream_Write_UINT16(s, pointer->xorBpp);
	Stream_Write_UINT16(s, pointer->cacheIndex);
	Stream_Write_UINT16(s, pointer->hotSpotX);
	Stream_Write_UINT16(s, pointer->hotSpotY);
	Stream_Write_UINT16(s, pointer->width);
	Stream_Write_UINT16(s, pointer->height);
	Stream_Write_UINT32(s, pointer->lengthAndMask);
	Stream_Write_UINT32(s, pointer->lengthXorMask);
	Stream_Write(s, pointer->xorMaskData, pointer->lengthXorMask);
	Stream_Write(s, pointer->andMaskData, pointer->lengthAndMask);
	Stream_Write_UINT8(s, 0); /* pad (1 byte) */
	return TRUE;
}