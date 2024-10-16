Bool gf_isom_get_last_producer_time_box(GF_ISOFile *file, GF_ISOTrackID *refTrackID, u64 *ntp, u64 *timestamp, Bool reset_info)
{
	if (!file) return GF_FALSE;
	if (refTrackID) *refTrackID = 0;
	if (ntp) *ntp = 0;
	if (timestamp) *timestamp = 0;

	if (file->last_producer_ref_time) {
		if (refTrackID) *refTrackID = file->last_producer_ref_time->refTrackID;
		if (ntp) *ntp = file->last_producer_ref_time->ntp;
		if (timestamp) *timestamp = file->last_producer_ref_time->timestamp;

		if (reset_info) {
			file->last_producer_ref_time = NULL;
		}
		return GF_TRUE;
	}
	return GF_FALSE;
}