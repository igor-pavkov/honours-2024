GF_Err grpi_box_read(GF_Box *s, GF_BitStream *bs)
{
	u16 gid_len;
	GF_OMADRMGroupIDBox *ptr = (GF_OMADRMGroupIDBox*)s;

	ISOM_DECREASE_SIZE(ptr, (1+2+2) );
	gid_len = gf_bs_read_u16(bs);
	ptr->GKEncryptionMethod = gf_bs_read_u8(bs);
	ptr->GKLength = gf_bs_read_u16(bs);

	if (ptr->size<gid_len+ptr->GKLength) return GF_ISOM_INVALID_FILE;

	ptr->GroupID = gf_malloc(sizeof(char)*(gid_len+1));
	if (!ptr->GroupID) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->GroupID, gid_len);
	ptr->GroupID[gid_len]=0;

	ptr->GroupKey = (char *)gf_malloc(sizeof(char)*ptr->GKLength);
	if (!ptr->GroupKey) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->GroupKey, ptr->GKLength);
	ISOM_DECREASE_SIZE(ptr, (gid_len+ptr->GKLength) );
	return GF_OK;
}