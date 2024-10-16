GF_Err gf_bifs_dec_node_list(GF_BifsDecoder * codec, GF_BitStream *bs, GF_Node *node, Bool is_proto)
{
	u8 flag;
	GF_Err e;
	u32 numBitsALL, numBitsDEF, field_all, field_ref, numProtoBits;
	GF_FieldInfo field;

	numProtoBits = numBitsALL = 0;
	if (codec->pCurrentProto) {
		numProtoBits = gf_get_bit_size(gf_sg_proto_get_field_count(codec->pCurrentProto) - 1);
		numBitsALL = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_ALL)-1);
	}
	numBitsDEF = gf_get_bit_size(gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DEF)-1);

	flag = gf_bs_read_int(bs, 1);
	while (!flag && (codec->LastError>=0)) {
		if (codec->pCurrentProto) {
			//IS'ed flag
			flag = gf_bs_read_int(bs, 1);
			if (flag) {
				//get field index in ALL mode for node
				field_ref = gf_bs_read_int(bs, numBitsALL);
				//get field index in ALL mode for proto
				field_all = gf_bs_read_int(bs, numProtoBits);
				e = gf_node_get_field(node, field_ref, &field);
				if (e) return e;
				e = BD_SetProtoISed(codec, field_all, node, field_ref);
				if (e) return e;
				flag = gf_bs_read_int(bs, 1);
				continue;
			}
		}

		//fields are coded in DEF mode
		field_ref = gf_bs_read_int(bs, numBitsDEF);
		e = gf_bifs_get_field_index(node, field_ref, GF_SG_FIELD_CODING_DEF, &field_all);
		if (e) return e;
		e = gf_node_get_field(node, field_all, &field);
		if (e) return e;
		e = gf_bifs_dec_field(codec, bs, node, &field, GF_FALSE);
		if (e) return e;
		flag = gf_bs_read_int(bs, 1);

		if (is_proto) gf_sg_proto_mark_field_loaded(node, &field);
	}
	return codec->LastError;
}