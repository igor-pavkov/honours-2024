GF_Err gf_isom_get_y3d_info(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescriptionIndex, GF_ISOM_Y3D_Info *info)
{
	GF_SampleEntryBox *ent;
	GF_TrackBox *trak;
	Bool found = GF_FALSE;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !info) return GF_BAD_PARAM;

	ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex-1);
	if (!ent) return GF_BAD_PARAM;

	memset(info, 0, sizeof(GF_ISOM_Y3D_Info));

	GF_Stereo3DBox *st3d = (GF_Stereo3DBox *) gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_ST3D);
	if (st3d) {
		found = GF_TRUE;
		info->stereo_type = st3d->stereo_type;
	}

	GF_Box *sv3d = gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_SV3D);
	if (!sv3d) {
		return found ? GF_OK : GF_NOT_FOUND;
	}
	GF_SphericalVideoInfoBox *svhd = (GF_SphericalVideoInfoBox *) gf_isom_box_find_child(sv3d->child_boxes, GF_ISOM_BOX_TYPE_SVHD);
	if (svhd && strlen(svhd->string)) info->meta_data = svhd->string;

	GF_Box *proj = gf_isom_box_find_child(sv3d->child_boxes, GF_ISOM_BOX_TYPE_PROJ);
	if (!proj)
		return found ? GF_OK : GF_NOT_FOUND;

	GF_ProjectionHeaderBox *projh = (GF_ProjectionHeaderBox *) gf_isom_box_find_child(proj->child_boxes, GF_ISOM_BOX_TYPE_PRHD);
	if (projh) {
		info->yaw = projh->yaw;
		info->pitch = projh->pitch;
		info->roll = projh->roll;
		info->pose_present = GF_TRUE;
	}

	GF_ProjectionTypeBox *projt = (GF_ProjectionTypeBox *) gf_isom_box_find_child(proj->child_boxes, GF_ISOM_BOX_TYPE_CBMP);
	if (projt) {
		info->layout = projt->layout;
		info->padding = projt->padding;
		info->projection_type = 1;
	} else {
		projt = (GF_ProjectionTypeBox *) gf_isom_box_find_child(proj->child_boxes, GF_ISOM_BOX_TYPE_EQUI);
		if (projt) {
			info->top = projt->bounds_top;
			info->bottom = projt->bounds_bottom;
			info->left = projt->bounds_left;
			info->right = projt->bounds_right;
			info->projection_type = 2;
		} else {
			projt = (GF_ProjectionTypeBox *) gf_isom_box_find_child(proj->child_boxes, GF_ISOM_BOX_TYPE_EQUI);
			if (projt) {
				info->projection_type = 3;
			}
		}
	}
	return GF_OK;
}