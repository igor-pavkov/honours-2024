GF_Err gf_isom_get_pssh_info(GF_ISOFile *file, u32 pssh_index, bin128 SystemID, u32 *version, u32 *KID_count, const bin128 **KIDs, const u8 **private_data, u32 *private_data_size)
{
	u32 count=1;
	u32 i=0;
	GF_ProtectionSystemHeaderBox *pssh=NULL;
	if (file->moov) {
		while ((pssh = (GF_ProtectionSystemHeaderBox *)gf_list_enum(file->moov->child_boxes, &i))) {
			if (pssh->type != GF_ISOM_BOX_TYPE_PSSH) continue;
			if (count == pssh_index) break;
			count++;
		}
	}
	if (!pssh && file->meta) {
		while ((pssh = (GF_ProtectionSystemHeaderBox *)gf_list_enum(file->meta->child_boxes, &i))) {
			if (pssh->type != GF_ISOM_BOX_TYPE_PSSH) continue;
			if (count == pssh_index) break;
			count++;
		}
	}
	if (!pssh) return GF_BAD_PARAM;

	if (SystemID) memcpy(SystemID, pssh->SystemID, 16);
	if (version) *version = pssh->version;
	if (KID_count) *KID_count = pssh->KID_count;
	if (KIDs) *KIDs = (const bin128 *) pssh->KIDs;
	if (private_data_size) *private_data_size = pssh->private_data_size;
	if (private_data) *private_data = pssh->private_data;
	return GF_OK;
}