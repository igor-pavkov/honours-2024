gdIOCtx * gdNewDynamicCtx (int initialSize, void *data)
{
	return gdNewDynamicCtxEx(initialSize, data, 1);
}