static bool updateGCB(GifInfo *info, uint_fast32_t *lastAllocatedGCBIndex) {
	if (*lastAllocatedGCBIndex < info->gifFilePtr->ImageCount) {
		GraphicsControlBlock *tmpInfos = reallocarray(info->controlBlock, info->gifFilePtr->ImageCount + 1, sizeof(GraphicsControlBlock));
		if (tmpInfos == NULL) {
			info->gifFilePtr->Error = D_GIF_ERR_NOT_ENOUGH_MEM;
			return false;
		}
		*lastAllocatedGCBIndex = info->gifFilePtr->ImageCount;
		info->controlBlock = tmpInfos;
		setGCBDefaults(&info->controlBlock[info->gifFilePtr->ImageCount]);
	}
	return true;
}