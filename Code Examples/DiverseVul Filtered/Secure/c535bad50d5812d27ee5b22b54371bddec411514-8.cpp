static GF_Err BM_ParseProtoDelete(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	u32 flag, count;
	GF_Command *com = gf_sg_command_new(codec->current_graph, GF_SG_PROTO_DELETE);
	flag = gf_bs_read_int(bs, 1);
	if (flag) {
		count = 0;
		flag = gf_bs_read_int(bs, 1);
		while (flag) {
			com->del_proto_list = (u32*)gf_realloc(com->del_proto_list, sizeof(u32) * (com->del_proto_list_size+1));
			com->del_proto_list[count] = gf_bs_read_int(bs, codec->info->config.ProtoIDBits);
			com->del_proto_list_size++;
			flag = gf_bs_read_int(bs, 1);
		}
	} else {
		flag = gf_bs_read_int(bs, 5);
		com->del_proto_list_size = gf_bs_read_int(bs, flag);
		com->del_proto_list = (u32*)gf_realloc(com->del_proto_list, sizeof(u32) * (com->del_proto_list_size));
		flag = 0;
		while (flag<com->del_proto_list_size) {
			com->del_proto_list[flag] = gf_bs_read_int(bs, codec->info->config.ProtoIDBits);
			flag++;
		}
	}
	gf_list_add(com_list, com);
	return GF_OK;
}