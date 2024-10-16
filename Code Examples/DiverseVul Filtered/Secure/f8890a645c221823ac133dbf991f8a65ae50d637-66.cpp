static BOOL update_write_bitmap_data(rdpUpdate* update, wStream* s, BITMAP_DATA* bitmapData)
{
	if (!Stream_EnsureRemainingCapacity(s, 64 + bitmapData->bitmapLength))
		return FALSE;

	if (update->autoCalculateBitmapData)
	{
		bitmapData->flags = 0;
		bitmapData->cbCompFirstRowSize = 0;

		if (bitmapData->compressed)
			bitmapData->flags |= BITMAP_COMPRESSION;

		if (update->context->settings->NoBitmapCompressionHeader)
		{
			bitmapData->flags |= NO_BITMAP_COMPRESSION_HDR;
			bitmapData->cbCompMainBodySize = bitmapData->bitmapLength;
		}
	}

	Stream_Write_UINT16(s, bitmapData->destLeft);
	Stream_Write_UINT16(s, bitmapData->destTop);
	Stream_Write_UINT16(s, bitmapData->destRight);
	Stream_Write_UINT16(s, bitmapData->destBottom);
	Stream_Write_UINT16(s, bitmapData->width);
	Stream_Write_UINT16(s, bitmapData->height);
	Stream_Write_UINT16(s, bitmapData->bitsPerPixel);
	Stream_Write_UINT16(s, bitmapData->flags);
	Stream_Write_UINT16(s, bitmapData->bitmapLength);

	if (bitmapData->flags & BITMAP_COMPRESSION)
	{
		if (!(bitmapData->flags & NO_BITMAP_COMPRESSION_HDR))
		{
			Stream_Write_UINT16(s,
			                    bitmapData->cbCompFirstRowSize); /* cbCompFirstRowSize (2 bytes) */
			Stream_Write_UINT16(s,
			                    bitmapData->cbCompMainBodySize); /* cbCompMainBodySize (2 bytes) */
			Stream_Write_UINT16(s, bitmapData->cbScanWidth);     /* cbScanWidth (2 bytes) */
			Stream_Write_UINT16(s,
			                    bitmapData->cbUncompressedSize); /* cbUncompressedSize (2 bytes) */
		}

		Stream_Write(s, bitmapData->bitmapDataStream, bitmapData->bitmapLength);
	}
	else
	{
		Stream_Write(s, bitmapData->bitmapDataStream, bitmapData->bitmapLength);
	}

	return TRUE;
}