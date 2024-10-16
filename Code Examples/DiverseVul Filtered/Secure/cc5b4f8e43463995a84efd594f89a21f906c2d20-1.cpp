static int readExtensions(int ExtFunction, GifByteType *ExtData, GifInfo *info) {
	if (ExtData == NULL) {
		return GIF_OK;
	}
	if (ExtFunction == GRAPHICS_EXT_FUNC_CODE) {
		GraphicsControlBlock *GCB = &info->controlBlock[info->gifFilePtr->ImageCount];
		if (DGifExtensionToGCB(ExtData[0], ExtData + 1, GCB) == GIF_ERROR) {
			return GIF_ERROR;
		}

		GCB->DelayTime = GCB->DelayTime > 1 ? GCB->DelayTime * 10 : DEFAULT_FRAME_DURATION_MS;
	}
	else if (ExtFunction == COMMENT_EXT_FUNC_CODE) {
		if (getComment(ExtData, info) == GIF_ERROR) {
			info->gifFilePtr->Error = D_GIF_ERR_NOT_ENOUGH_MEM;
			return GIF_ERROR;
		}
	}
	else if (ExtFunction == APPLICATION_EXT_FUNC_CODE) {
		char const *string = (char const *) (ExtData + 1);
		if (strncmp("NETSCAPE2.0", string, ExtData[0]) == 0
		    || strncmp("ANIMEXTS1.0", string, ExtData[0]) == 0) {
			if (DGifGetExtensionNext(info->gifFilePtr, &ExtData) == GIF_ERROR) {
				return GIF_ERROR;
			}
			if (ExtData && ExtData[0] == 3 && ExtData[1] == 1) {
				uint_fast16_t loopCount = (uint_fast16_t) (ExtData[2] + (ExtData[3] << 8));
				if (loopCount) {
					loopCount++;
				}
				info->loopCount = loopCount;
			}
		}
	}
	return GIF_OK;
}