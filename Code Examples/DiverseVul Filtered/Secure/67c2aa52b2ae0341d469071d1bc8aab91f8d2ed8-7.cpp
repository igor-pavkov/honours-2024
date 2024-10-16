static BOOL update_read_memblt_order(wStream* s, const ORDER_INFO* orderInfo, MEMBLT_ORDER* memblt)
{
	if (!s || !orderInfo || !memblt)
		return FALSE;

	ORDER_FIELD_UINT16(1, memblt->cacheId);
	ORDER_FIELD_COORD(2, memblt->nLeftRect);
	ORDER_FIELD_COORD(3, memblt->nTopRect);
	ORDER_FIELD_COORD(4, memblt->nWidth);
	ORDER_FIELD_COORD(5, memblt->nHeight);
	ORDER_FIELD_BYTE(6, memblt->bRop);
	ORDER_FIELD_COORD(7, memblt->nXSrc);
	ORDER_FIELD_COORD(8, memblt->nYSrc);
	ORDER_FIELD_UINT16(9, memblt->cacheIndex);
	memblt->colorIndex = (memblt->cacheId >> 8);
	memblt->cacheId = (memblt->cacheId & 0xFF);
	memblt->bitmap = NULL;
	return TRUE;
}