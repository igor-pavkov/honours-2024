static BOOL update_read_mem3blt_order(wStream* s, const ORDER_INFO* orderInfo,
                                      MEM3BLT_ORDER* mem3blt)
{
	ORDER_FIELD_UINT16(1, mem3blt->cacheId);
	ORDER_FIELD_COORD(2, mem3blt->nLeftRect);
	ORDER_FIELD_COORD(3, mem3blt->nTopRect);
	ORDER_FIELD_COORD(4, mem3blt->nWidth);
	ORDER_FIELD_COORD(5, mem3blt->nHeight);
	ORDER_FIELD_BYTE(6, mem3blt->bRop);
	ORDER_FIELD_COORD(7, mem3blt->nXSrc);
	ORDER_FIELD_COORD(8, mem3blt->nYSrc);
	ORDER_FIELD_COLOR(orderInfo, s, 9, &mem3blt->backColor);
	ORDER_FIELD_COLOR(orderInfo, s, 10, &mem3blt->foreColor);

	if (!update_read_brush(s, &mem3blt->brush, orderInfo->fieldFlags >> 10))
		return FALSE;

	ORDER_FIELD_UINT16(16, mem3blt->cacheIndex);
	mem3blt->colorIndex = (mem3blt->cacheId >> 8);
	mem3blt->cacheId = (mem3blt->cacheId & 0xFF);
	mem3blt->bitmap = NULL;
	return TRUE;
}