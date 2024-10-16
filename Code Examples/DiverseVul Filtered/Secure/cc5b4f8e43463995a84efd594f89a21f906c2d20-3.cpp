static int getComment(GifByteType *Bytes, GifInfo *info) {
	unsigned int len = (unsigned int) Bytes[0];
	size_t offset = info->comment != NULL ? strlen(info->comment) : 0;
	char *ret = reallocarray(info->comment, len + offset + 1, sizeof(char));
	if (ret != NULL) {
		memcpy(ret + offset, &Bytes[1], len);
		ret[len + offset] = 0;
		info->comment = ret;
		return GIF_OK;
	}
	info->gifFilePtr->Error = D_GIF_ERR_NOT_ENOUGH_MEM;
	return GIF_ERROR;
}