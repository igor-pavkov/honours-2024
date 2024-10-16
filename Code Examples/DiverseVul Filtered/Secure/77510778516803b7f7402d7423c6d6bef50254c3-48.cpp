GF_Err hdlr_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 cookie;
	GF_HandlerBox *ptr = (GF_HandlerBox *)s;

	ISOM_DECREASE_SIZE(ptr, 20);
	ptr->reserved1 = gf_bs_read_u32(bs);
	ptr->handlerType = gf_bs_read_u32(bs);
	gf_bs_read_data(bs, (char*)ptr->reserved2, 12);

	cookie = gf_bs_get_cookie(bs);
	if (ptr->handlerType==GF_ISOM_MEDIA_VISUAL)
		cookie |= GF_ISOM_BS_COOKIE_VISUAL_TRACK;
	else
		cookie &= ~GF_ISOM_BS_COOKIE_VISUAL_TRACK;
	gf_bs_set_cookie(bs, cookie);

	if (ptr->size) {
		u32 name_size = (u32) ptr->size;
		if (name_size < 1) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in hdlr\n", ptr->size));
			return GF_ISOM_INVALID_FILE;
		}
		ptr->nameUTF8 = (char*)gf_malloc(name_size);
		if (!ptr->nameUTF8) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->nameUTF8, name_size);

		//patch for old QT files - we cannot rely on checking if str[0]==len(str+1) since we may have
		//cases where the first character of the string decimal value is indeed the same as the string length!!
		//we had this issue with encryption_import test
		//we therefore only check if last char is null, and if not so assume old QT style
		if (ptr->nameUTF8[name_size-1]) {
			if (name_size > 1)
				memmove(ptr->nameUTF8, ptr->nameUTF8+1, sizeof(char) * (u32) (name_size-1) );
			ptr->nameUTF8[name_size-1] = 0;
			ptr->store_counted_string = GF_TRUE;
		}
	}
	return GF_OK;
}