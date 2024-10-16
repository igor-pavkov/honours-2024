GF_Err BM_ParseCommand(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u8 go, type;
	GF_Err e;
	go = 1;
	e = GF_OK;
	GF_SceneGraph *cur_graph = codec->current_graph;
	GF_Proto *cur_proto = codec->pCurrentProto;

	codec->LastError = GF_OK;
	while (go) {
		type = gf_bs_read_int(bs, 2);
		switch (type) {
		case 0:
			e = BM_ParseInsert(codec, bs, com_list);
			break;
		case 1:
			e = BM_ParseDelete(codec, bs, com_list);
			break;
		case 2:
			e = BM_ParseReplace(codec, bs, com_list);
			break;
		case 3:
			e = BM_SceneReplace(codec, bs, com_list);
			break;
		}
		if (e) break;
		go = gf_bs_read_int(bs, 1);
	}
	while (gf_list_count(codec->QPs)) {
		gf_bifs_dec_qp_remove(codec, GF_TRUE);
	}

	codec->current_graph = cur_graph;
	codec->pCurrentProto = cur_proto;
	return e;
}