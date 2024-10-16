static INLINE BOOL ORDER_FIELD_COLOR(const ORDER_INFO* orderInfo, wStream* s, UINT32 NO,
                                     UINT32* TARGET)
{
	if (!TARGET || !orderInfo)
		return FALSE;

	if ((orderInfo->fieldFlags & (1 << (NO - 1))) && !update_read_color(s, TARGET))
		return FALSE;

	return TRUE;
}