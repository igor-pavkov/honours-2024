POINTER_LARGE_UPDATE* update_read_pointer_large(rdpUpdate* update, wStream* s)
{
	POINTER_LARGE_UPDATE* pointer = calloc(1, sizeof(POINTER_LARGE_UPDATE));

	if (!pointer)
		goto fail;

	if (!_update_read_pointer_large(s, pointer))
		goto fail;

	return pointer;
fail:
	free_pointer_large_update(update->context, pointer);
	return NULL;
}