static int gdReallocDynamic (dynamicPtr * dp, int required)
{
	void *newPtr;

	/* First try gdRealloc(). If that doesn't work, make a new memory block and copy. */
	if ((newPtr = gdRealloc(dp->data, required))) {
		dp->realSize = required;
		dp->data = newPtr;
		return TRUE;
	}

	/* create a new pointer */
	newPtr = gdMalloc(required);

	/* copy the old data into it */
	memcpy(newPtr, dp->data, dp->logicalSize);
	gdFree(dp->data);
	dp->data = newPtr;

	dp->realSize = required;

	return TRUE;
}