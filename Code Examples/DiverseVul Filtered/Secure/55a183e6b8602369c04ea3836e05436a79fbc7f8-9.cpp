GF_Err gf_bifs_dec_node_mask(GF_BifsDecoder * codec, GF_BitStream *bs, GF_Node *node, Bool is_proto)
{
	u32 i, numFields, numProtoFields, index, flag, nbBits;
	GF_Err e;
	GF_FieldInfo field;

	//proto coding
	if (codec->pCurrentProto) {
		numFields = gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_ALL);
		numProtoFields = gf_sg_proto_get_field_count(codec->pCurrentProto);
		nbBits = gf_get_bit_size(numProtoFields-1);

		for (i=0; i<numFields; i++) {
			flag = gf_bs_read_int(bs, 1);
			if (!flag) continue;
			flag = gf_bs_read_int(bs, 1);
			//IS'ed field, create route for binding to Proto declaration
			if (flag) {
				//reference index of our IS'ed proto field
				flag = gf_bs_read_int(bs, nbBits);
				e = gf_node_get_field(node, i, &field);
				if (e) return e;
				e = BD_SetProtoISed(codec, flag, node, i);
			}
			//regular field, parse it (nb: no contextual coding for protos in maskNode,
			//all node fields are coded
			else {
				e = gf_node_get_field(node, i, &field);
				if (e) return e;
				e = gf_bifs_dec_field(codec, bs, node, &field, GF_FALSE);
			}
			if (e) return e;
		}
	}
	//regular coding
	else {
		numFields = gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DEF);
		for (i=0; i<numFields; i++) {
			flag = gf_bs_read_int(bs, 1);
			if (!flag) continue;
			gf_bifs_get_field_index(node, i, GF_SG_FIELD_CODING_DEF, &index);
			e = gf_node_get_field(node, index, &field);
			if (e) return e;
			e = gf_bifs_dec_field(codec, bs, node, &field, GF_FALSE);
			if (e) return e;

			if (is_proto) gf_sg_proto_mark_field_loaded(node, &field);
		}
	}
	return GF_OK;
}