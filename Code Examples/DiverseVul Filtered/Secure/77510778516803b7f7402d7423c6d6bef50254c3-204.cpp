GF_Err traf_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *) s;

	//Header first
	gf_isom_check_position(s, (GF_Box *)ptr->tfhd, &pos);
	gf_isom_check_position_list(s, ptr->sub_samples, &pos);

	gf_isom_check_position(s, (GF_Box *)ptr->tfdt, &pos);

	//cmaf-like
	if (ptr->truns_first) {
		gf_isom_check_position_list(s, ptr->TrackRuns, &pos);
		gf_isom_check_position_list(s, ptr->sai_sizes, &pos);
		gf_isom_check_position_list(s, ptr->sai_offsets, &pos);
		//senc MUST be after saio in GPAC, as senc writing uses info from saio writing
		gf_isom_check_position(s, (GF_Box *)ptr->sample_encryption, &pos);
		gf_isom_check_position_list(s, ptr->sampleGroupsDescription, &pos);
		gf_isom_check_position_list(s, ptr->sampleGroups, &pos);
		//subsamples will be last
	} else {
		gf_isom_check_position_list(s, ptr->sampleGroupsDescription, &pos);
		gf_isom_check_position_list(s, ptr->sampleGroups, &pos);
		gf_isom_check_position_list(s, ptr->sai_sizes, &pos);
		gf_isom_check_position_list(s, ptr->sai_offsets, &pos);
		gf_isom_check_position(s, (GF_Box *)ptr->sample_encryption, &pos);
		gf_isom_check_position_list(s, ptr->TrackRuns, &pos);
	}

	//when sdtp is present (smooth-like) write it after the trun box
	gf_isom_check_position(s, (GF_Box *)ptr->sdtp, &pos);

	//tfxd should be last ...
	if (ptr->tfxd)
		gf_isom_check_position(s, (GF_Box *)ptr->tfxd, &pos);
	return GF_OK;
}