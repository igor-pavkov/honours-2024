GF_Err hdlr_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_HandlerBox *ptr = (GF_HandlerBox *)s;

	ptr->reserved1 = gf_bs_read_u32(bs);
	ptr->handlerType = gf_bs_read_u32(bs);
	gf_bs_read_data(bs, (char*)ptr->reserved2, 12);

	ISOM_DECREASE_SIZE(ptr, 20);

	if (ptr->size) {
		ptr->nameUTF8 = (char*)gf_malloc((u32) ptr->size);
		if (ptr->nameUTF8 == NULL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->nameUTF8, (u32) ptr->size);

		//patch for old QT files - we cannot rely on checking if str[0]==len(str+1) since we may have
		//cases where the first character of the string decimal value is indeed the same as the string length!!
		//we had this issue with encryption_import test
		//we therefore only check if last char is null, and if not so assume old QT style
		if (ptr->nameUTF8[ptr->size-1]) {
			memmove(ptr->nameUTF8, ptr->nameUTF8+1, sizeof(char) * (u32) (ptr->size-1) );
			ptr->nameUTF8[ptr->size-1] = 0;
			ptr->store_counted_string = GF_TRUE;
		}
	}
	return GF_OK;
}