GF_Err gf_cenc_set_pssh(GF_ISOFile *file, bin128 systemID, u32 version, u32 KID_count, bin128 *KIDs, u8 *data, u32 len, u32 pssh_mode)
{
	GF_ProtectionSystemHeaderBox *pssh = NULL;
	GF_PIFFProtectionSystemHeaderBox *pssh_piff = NULL;
	u32 i=0;
	GF_Box *a;
	GF_List **child_boxes = NULL;

	if (pssh_mode==2) {
		if (!file->meta) return GF_BAD_PARAM;
		if (!file->meta->child_boxes) file->meta->child_boxes = gf_list_new();
		child_boxes = &file->meta->child_boxes;
	} else if (file->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY) {
		if (!file->moof) return GF_BAD_PARAM;
		if (!file->moof->PSSHs) file->moof->PSSHs = gf_list_new();
		child_boxes = &file->moof->PSSHs;
	} else {
		if (!file->moov) return GF_BAD_PARAM;
		if (!file->moov->child_boxes) file->moov->child_boxes = gf_list_new();
		child_boxes = &file->moov->child_boxes;
	}

	while ((a = gf_list_enum(*child_boxes, &i))) {
		GF_UUIDBox *uuid = (GF_UUIDBox *)a;
		if (a->type==GF_ISOM_BOX_TYPE_PSSH) {
			pssh = (GF_ProtectionSystemHeaderBox *)a;
			if (!memcmp(pssh->SystemID, systemID, sizeof(bin128))) break;
			pssh = NULL;
		} else if ((a->type==GF_ISOM_BOX_TYPE_UUID) && (uuid->internal_4cc==GF_ISOM_BOX_UUID_PSSH)) {
			pssh_piff = (GF_PIFFProtectionSystemHeaderBox *)a;
			if (!memcmp(pssh_piff->SystemID, systemID, sizeof(bin128))) break;
			pssh_piff = NULL;
		}
	}
	//we had a pssh with same ID but different private data, keep both...
	if (pssh && pssh->private_data && len && memcmp(pssh->private_data, data, sizeof(char)*len) ) {
		pssh = NULL;
	}
	else if (pssh_piff && pssh_piff->private_data && len && memcmp(pssh_piff->private_data, data, sizeof(char)*len) ) {
		pssh_piff = NULL;
	}

	if (!pssh && !pssh_piff) {
		if (pssh_mode==1) {
			pssh_piff = (GF_PIFFProtectionSystemHeaderBox *)gf_isom_box_new_parent(child_boxes, GF_ISOM_BOX_UUID_PSSH);
			if (!pssh_piff) return GF_IO_ERR;
			memcpy((char *)pssh_piff->SystemID, systemID, sizeof(bin128));
			pssh_piff->version = version;
		} else {
			pssh = (GF_ProtectionSystemHeaderBox *)gf_isom_box_new_parent(child_boxes, GF_ISOM_BOX_TYPE_PSSH);
			if (!pssh) return GF_IO_ERR;
			memcpy((char *)pssh->SystemID, systemID, sizeof(bin128));
			pssh->version = version;
		}
	}

	if (pssh && KID_count) {
		u32 j;
		for (j=0; j<KID_count; j++) {
			Bool found = GF_FALSE;
			for (i=0; i<pssh->KID_count; i++) {
				if (!memcmp(pssh->KIDs[i], KIDs[j], sizeof(bin128))) found = GF_TRUE;
			}

			if (!found) {
				pssh->KIDs = gf_realloc(pssh->KIDs, sizeof(bin128) * (pssh->KID_count+1));
				if (!pssh->KIDs) return GF_OUT_OF_MEM;
				memcpy(pssh->KIDs[pssh->KID_count], KIDs[j], sizeof(bin128));
				pssh->KID_count++;
			}
		}
		if (!pssh->version)
			pssh->version = 1;
	}

	if (pssh) {
		if (!pssh->private_data_size) {
			pssh->private_data_size = len;
			if (len) {
				if (!pssh->private_data) {
					pssh->private_data = (u8 *)gf_malloc(pssh->private_data_size*sizeof(char));
					if (!pssh->private_data) return GF_OUT_OF_MEM;
				}
				memcpy((char *)pssh->private_data, data, pssh->private_data_size);
			}
		}
	} else if (pssh_piff) {
		if (!pssh_piff->private_data_size) {
			pssh_piff->private_data_size = len;
			if (len) {
				if (!pssh_piff->private_data) {
					pssh_piff->private_data = (u8 *)gf_malloc(pssh_piff->private_data_size*sizeof(char));
					if (!pssh_piff->private_data) return GF_OUT_OF_MEM;
				}
				memcpy((char *)pssh_piff->private_data, data, pssh_piff->private_data_size);
			}
		}
	}
	return GF_OK;
}