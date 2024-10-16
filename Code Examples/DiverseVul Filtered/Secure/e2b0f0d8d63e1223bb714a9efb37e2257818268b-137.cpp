static sdp_record_t *avrcp_ct_record(void)
{
	sdp_list_t *svclass_id, *pfseq, *apseq, *apseq1, *root;
	uuid_t root_uuid, l2cap, avctp, avrct, avrctr;
	sdp_profile_desc_t profile[1];
	sdp_list_t *aproto, *aproto1, *proto[2], *proto1[2];
	sdp_record_t *record;
	sdp_data_t *psm[2], *version, *features;
	uint16_t lp = AVCTP_CONTROL_PSM, ap = AVCTP_BROWSING_PSM;
	uint16_t avctp_ver = 0x0103;
	uint16_t feat = ( AVRCP_FEATURE_CATEGORY_1 |
						AVRCP_FEATURE_CATEGORY_2 |
						AVRCP_FEATURE_CATEGORY_3 |
						AVRCP_FEATURE_CATEGORY_4 |
						AVRCP_FEATURE_BROWSING);

	record = sdp_record_alloc();
	if (!record)
		return NULL;

	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root = sdp_list_append(NULL, &root_uuid);
	sdp_set_browse_groups(record, root);

	/* Service Class ID List */
	sdp_uuid16_create(&avrct, AV_REMOTE_SVCLASS_ID);
	svclass_id = sdp_list_append(NULL, &avrct);
	sdp_uuid16_create(&avrctr, AV_REMOTE_CONTROLLER_SVCLASS_ID);
	svclass_id = sdp_list_append(svclass_id, &avrctr);
	sdp_set_service_classes(record, svclass_id);

	/* Protocol Descriptor List */
	sdp_uuid16_create(&l2cap, L2CAP_UUID);
	proto[0] = sdp_list_append(NULL, &l2cap);
	psm[0] = sdp_data_alloc(SDP_UINT16, &lp);
	proto[0] = sdp_list_append(proto[0], psm[0]);
	apseq = sdp_list_append(NULL, proto[0]);

	sdp_uuid16_create(&avctp, AVCTP_UUID);
	proto[1] = sdp_list_append(NULL, &avctp);
	version = sdp_data_alloc(SDP_UINT16, &avctp_ver);
	proto[1] = sdp_list_append(proto[1], version);
	apseq = sdp_list_append(apseq, proto[1]);

	aproto = sdp_list_append(NULL, apseq);
	sdp_set_access_protos(record, aproto);

	/* Additional Protocol Descriptor List */
	sdp_uuid16_create(&l2cap, L2CAP_UUID);
	proto1[0] = sdp_list_append(NULL, &l2cap);
	psm[1] = sdp_data_alloc(SDP_UINT16, &ap);
	proto1[0] = sdp_list_append(proto1[0], psm[1]);
	apseq1 = sdp_list_append(NULL, proto1[0]);

	sdp_uuid16_create(&avctp, AVCTP_UUID);
	proto1[1] = sdp_list_append(NULL, &avctp);
	proto1[1] = sdp_list_append(proto1[1], version);
	apseq1 = sdp_list_append(apseq1, proto1[1]);

	aproto1 = sdp_list_append(NULL, apseq1);
	sdp_set_add_access_protos(record, aproto1);

	/* Bluetooth Profile Descriptor List */
	sdp_uuid16_create(&profile[0].uuid, AV_REMOTE_PROFILE_ID);
	profile[0].version = AVRCP_CT_VERSION;
	pfseq = sdp_list_append(NULL, &profile[0]);
	sdp_set_profile_descs(record, pfseq);

	features = sdp_data_alloc(SDP_UINT16, &feat);
	sdp_attr_add(record, SDP_ATTR_SUPPORTED_FEATURES, features);

	sdp_set_info_attr(record, "AVRCP CT", NULL, NULL);

	free(psm[0]);
	free(psm[1]);
	free(version);
	sdp_list_free(proto[0], NULL);
	sdp_list_free(proto[1], NULL);
	sdp_list_free(apseq, NULL);
	sdp_list_free(proto1[0], NULL);
	sdp_list_free(proto1[1], NULL);
	sdp_list_free(aproto1, NULL);
	sdp_list_free(apseq1, NULL);
	sdp_list_free(pfseq, NULL);
	sdp_list_free(aproto, NULL);
	sdp_list_free(root, NULL);
	sdp_list_free(svclass_id, NULL);

	return record;
}