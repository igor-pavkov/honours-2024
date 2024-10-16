GF_HEVCConfig *gf_isom_lhvc_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_HEVCConfig *lhvc;
	GF_OperatingPointsInformation *oinf=NULL;
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	if (gf_isom_get_hevc_lhvc_type(the_file, trackNumber, DescriptionIndex)==GF_ISOM_HEVCTYPE_NONE)
		return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return NULL;
	if (!entry->lhvc_config) return NULL;
	lhvc = HEVC_DuplicateConfig(entry->lhvc_config->config);
	if (!lhvc) return NULL;

	gf_isom_get_oinf_info(the_file, trackNumber, &oinf);
	if (oinf) {
		LHEVC_ProfileTierLevel *ptl = (LHEVC_ProfileTierLevel *)gf_list_last(oinf->profile_tier_levels);
		if (ptl) {
			lhvc->profile_space  = ptl->general_profile_space;
			lhvc->tier_flag = ptl->general_tier_flag;
			lhvc->profile_idc = ptl->general_profile_idc;
			lhvc->general_profile_compatibility_flags = ptl->general_profile_compatibility_flags;
			lhvc->constraint_indicator_flags = ptl->general_constraint_indicator_flags;
		}
	}
	return lhvc;
}