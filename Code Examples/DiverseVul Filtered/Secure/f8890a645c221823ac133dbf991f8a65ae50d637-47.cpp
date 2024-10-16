static BOOL update_write_pointer_color(wStream* s, const POINTER_COLOR_UPDATE* pointer_color)
{
	if (!Stream_EnsureRemainingCapacity(s, 32 + pointer_color->lengthAndMask +
	                                           pointer_color->lengthXorMask))
		return FALSE;

	Stream_Write_UINT16(s, pointer_color->cacheIndex);
	Stream_Write_UINT16(s, pointer_color->xPos);
	Stream_Write_UINT16(s, pointer_color->yPos);
	Stream_Write_UINT16(s, pointer_color->width);
	Stream_Write_UINT16(s, pointer_color->height);
	Stream_Write_UINT16(s, pointer_color->lengthAndMask);
	Stream_Write_UINT16(s, pointer_color->lengthXorMask);

	if (pointer_color->lengthXorMask > 0)
		Stream_Write(s, pointer_color->xorMaskData, pointer_color->lengthXorMask);

	if (pointer_color->lengthAndMask > 0)
		Stream_Write(s, pointer_color->andMaskData, pointer_color->lengthAndMask);

	Stream_Write_UINT8(s, 0); /* pad (1 byte) */
	return TRUE;
}