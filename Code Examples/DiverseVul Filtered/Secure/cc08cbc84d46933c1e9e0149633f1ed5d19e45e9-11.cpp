static int appendDynamic (dynamicPtr * dp, const void *src, int size)
{
	int bytesNeeded;
	char *tmp;

	if (!dp->dataGood) {
		return FALSE;
	}

	/*  bytesNeeded = dp->logicalSize + size; */
	bytesNeeded = dp->pos + size;

	if (bytesNeeded > dp->realSize) {
		/* 2.0.21 */
		if (!dp->freeOK) {
			return FALSE;
		}
		gdReallocDynamic(dp, bytesNeeded * 2);
	}

	/* if we get here, we can be sure that we have enough bytes to copy safely */
	/*printf("Mem OK Size: %d, Pos: %d\n", dp->realSize, dp->pos); */

	tmp = (char *) dp->data;
	memcpy((void *) (tmp + (dp->pos)), src, size);
	dp->pos += size;

	if (dp->pos > dp->logicalSize) {
		dp->logicalSize = dp->pos;
	}

	return TRUE;
}