GF_Err BM_SceneReplace(GF_BifsDecoder *codec, GF_BitStream *bs, GF_List *com_list)
{
	GF_Command *com;
	GF_Node *backup_root;
	GF_List *backup_routes;
	GF_Err BD_DecSceneReplace(GF_BifsDecoder * codec, GF_BitStream *bs, GF_List *proto_list);

	backup_routes = codec->scenegraph->Routes;
	backup_root = codec->scenegraph->RootNode;
	com = gf_sg_command_new(codec->current_graph, GF_SG_SCENE_REPLACE);
	codec->scenegraph->Routes = gf_list_new();
	codec->current_graph = codec->scenegraph;
	codec->LastError = BD_DecSceneReplace(codec, bs, com->new_proto_list);
	com->use_names = codec->UseName;

	/*restore*/
	com->node = codec->scenegraph->RootNode;
	codec->scenegraph->RootNode = backup_root;
	gf_list_add(com_list, com);
	/*insert routes*/
	while (gf_list_count(codec->scenegraph->Routes)) {
		GF_Route *r = (GF_Route*)gf_list_get(codec->scenegraph->Routes, 0);
		GF_Command *ri = gf_sg_command_new(codec->current_graph, GF_SG_ROUTE_INSERT);
		gf_list_rem(codec->scenegraph->Routes, 0);
		ri->fromFieldIndex = r->FromField.fieldIndex;
		ri->fromNodeID = gf_node_get_id(r->FromNode);
		ri->toFieldIndex = r->ToField.fieldIndex;
		ri->toNodeID = gf_node_get_id(r->ToNode);
		if (r->ID) ri->RouteID = r->ID;
		ri->def_name = r->name ? gf_strdup(r->name) : NULL;
		gf_list_add(com_list, ri);
		gf_sg_route_del(r);
	}
	gf_list_del(codec->scenegraph->Routes);
	codec->scenegraph->Routes = backup_routes;
	return codec->LastError;
}