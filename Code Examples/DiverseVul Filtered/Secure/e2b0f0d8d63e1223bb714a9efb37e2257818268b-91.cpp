static sdp_record_t *avrcp_tg_record(void)
{
	sdp_list_t *svclass_id, *pfseq, *apseq, *root, *apseq_browsing;
	uuid_t root_uuid, l2cap, avctp, avrtg;
	sdp_profile_desc_t profile[1];
	sdp_list_t *aproto_control, *proto_control[2];
	sdp_record_t *record;
	sdp_data_t *psm_control, *version, *features, *psm_browsing;
	sdp_list_t *aproto_browsing, *proto_browsing[2] = {0};
	uint16_t lp = AVCTP_CONTROL_PSM;
	uint16_t lp_browsing = AVCTP_BROWSING_PSM;
	uint16_t avctp_ver = 0x0103;
	uint16_t feat = ( AVRCP_FEATURE_CATEGORY_1 |
					AVRCP_FEATURE_CATEGORY_2 |
					AVRCP_FEATURE_CATEGORY_3 |
					AVRCP_FEATURE_CATEGORY_4 |
					AVRCP_FEATURE_BROWSING |
					AVRCP_FEATURE_PLAYER_SETTINGS );

	record = sdp_record_alloc();
	if (!record)
		return NULL;

	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root = sdp_list_append(NULL, &root_uuid);
	sdp_set_browse_groups(record, root);

	/* Service Class ID List */
	sdp_uuid16_create(&avrtg, AV_REMOTE_TARGET_SVCLASS_ID);
	svclass_id = sdp_list_append(NULL, &avrtg);
	sdp_set_service_classes(record, svclass_id);

	/* Protocol Descriptor List */
	sdp_uuid16_create(&l2cap, L2CAP_UUID);
	proto_control[0] = sdp_list_append(NULL, &l2cap);
	psm_control = sdp_data_alloc(SDP_UINT16, &lp);
	proto_control[0] = sdp_list_append(proto_control[0], psm_control);
	apseq = sdp_list_append(NULL, proto_control[0]);

	sdp_uuid16_create(&avctp, AVCTP_UUID);
	proto_control[1] = sdp_list_append(NULL, &avctp);
	version = sdp_data_alloc(SDP_UINT16, &avctp_ver);
	proto_control[1] = sdp_list_append(proto_control[1], version);
	apseq = sdp_list_append(apseq, proto_control[1]);

	aproto_control = sdp_list_append(NULL, apseq);
	sdp_set_access_protos(record, aproto_control);
	proto_browsing[0] = sdp_list_append(NULL, &l2cap);
	psm_browsing = sdp_data_alloc(SDP_UINT16, &lp_browsing);
	proto_browsing[0] = sdp_list_append(proto_browsing[0], psm_browsing);
	apseq_browsing = sdp_list_append(NULL, proto_browsing[0]);

	proto_browsing[1] = sdp_list_append(NULL, &avctp);
	proto_browsing[1] = sdp_list_append(proto_browsing[1], version);
	apseq_browsing = sdp_list_append(apseq_browsing, proto_browsing[1]);

	aproto_browsing = sdp_list_append(NULL, apseq_browsing);
	sdp_set_add_access_protos(record, aproto_browsing);

	/* Bluetooth Profile Descriptor List */
	sdp_uuid16_create(&profile[0].uuid, AV_REMOTE_PROFILE_ID);
	profile[0].version = AVRCP_TG_VERSION;
	pfseq = sdp_list_append(NULL, &profile[0]);
	sdp_set_profile_descs(record, pfseq);

	features = sdp_data_alloc(SDP_UINT16, &feat);
	sdp_attr_add(record, SDP_ATTR_SUPPORTED_FEATURES, features);

	sdp_set_info_attr(record, "AVRCP TG", NULL, NULL);

	free(psm_browsing);
	sdp_list_free(proto_browsing[0], NULL);
	sdp_list_free(proto_browsing[1], NULL);
	sdp_list_free(apseq_browsing, NULL);
	sdp_list_free(aproto_browsing, NULL);

	free(psm_control);
	free(version);
	sdp_list_free(proto_control[0], NULL);
	sdp_list_free(proto_control[1], NULL);
	sdp_list_free(apseq, NULL);
	sdp_list_free(aproto_control, NULL);
	sdp_list_free(pfseq, NULL);
	sdp_list_free(root, NULL);
	sdp_list_free(svclass_id, NULL);

	return record;
}