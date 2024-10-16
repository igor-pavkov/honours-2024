GF_Node *gf_bifs_dec_node(GF_BifsDecoder * codec, GF_BitStream *bs, u32 NDT_Tag)
{
	u32 nodeID, NDTBits, node_type, node_tag, ProtoID, BVersion;
	Bool skip_init, reset_qp14;
	GF_Node *new_node;
	GF_Err e;
	GF_Proto *proto;
	void SetupConditional(GF_BifsDecoder *codec, GF_Node *node);

	//to store the UseName
	char name[1000];

#if 0
	/*should only happen with inputSensor, in which case this is BAAAAD*/
	if (!codec->info) {
		codec->LastError = GF_BAD_PARAM;
		return NULL;
	}
#endif


	BVersion = GF_BIFS_V1;

	/*this is a USE statement*/
	if (gf_bs_read_int(bs, 1)) {
		nodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		/*NULL node is encoded as USE with ID = all bits to 1*/
		if (nodeID == (u32) (1<<codec->info->config.NodeIDBits))
			return NULL;
		//find node
		new_node = gf_sg_find_node(codec->current_graph, nodeID);

		//check node is allowed for the given NDT
		if (new_node && !gf_node_in_table(new_node, NDT_Tag)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[BIFS] Node %s not allowed as field/child of NDT type %d\n", gf_node_get_class_name(new_node), NDT_Tag));
			codec->LastError = GF_SG_UNKNOWN_NODE;
			return NULL;
		}

		if (!new_node) {
			codec->LastError = GF_SG_UNKNOWN_NODE;
		} else {
			/*restore QP14 length*/
			switch (gf_node_get_tag(new_node)) {
			case TAG_MPEG4_Coordinate:
			{
				u32 nbCoord = ((M_Coordinate *)new_node)->point.count;
				gf_bifs_dec_qp14_enter(codec, GF_TRUE);
				gf_bifs_dec_qp14_set_length(codec, nbCoord);
				gf_bifs_dec_qp14_enter(codec, GF_FALSE);
			}
			break;
			case TAG_MPEG4_Coordinate2D:
			{
				u32 nbCoord = ((M_Coordinate2D *)new_node)->point.count;
				gf_bifs_dec_qp14_enter(codec, GF_TRUE);
				gf_bifs_dec_qp14_set_length(codec, nbCoord);
				gf_bifs_dec_qp14_enter(codec, GF_FALSE);
			}
			break;
			}
		}
		return new_node;
	}

	//this is a new node
	nodeID = 0;
	name[0] = 0;
	node_tag = 0;
	proto = NULL;

	//browse all node groups
	while (1) {
		NDTBits = gf_bifs_get_ndt_bits(NDT_Tag, BVersion);
		/*this happens in replacescene where no top-level node is present (externProto)*/
		if ((BVersion==1) && (NDTBits > 8 * gf_bs_available(bs)) ) {
			codec->LastError = GF_OK;
			return NULL;
		}

		node_type = gf_bs_read_int(bs, NDTBits);
		if (node_type) break;

		//increment BIFS version
		BVersion += 1;
		//not supported
		if (BVersion > GF_BIFS_NUM_VERSION) {
			codec->LastError = GF_BIFS_UNKNOWN_VERSION;
			return NULL;
		}
	}
	if (BVersion==2 && node_type==1) {
		ProtoID = gf_bs_read_int(bs, codec->info->config.ProtoIDBits);
		/*look in current graph for the proto - this may be a proto graph*/
		proto = gf_sg_find_proto(codec->current_graph, ProtoID, NULL);
		/*this was in proto so look in main scene*/
		if (!proto && codec->current_graph != codec->scenegraph)
			proto = gf_sg_find_proto(codec->scenegraph, ProtoID, NULL);

		if (!proto) {
			codec->LastError = GF_SG_UNKNOWN_NODE;
			return NULL;
		}
	} else {
		node_tag = gf_bifs_ndt_get_node_type(NDT_Tag, node_type, BVersion);
	}

	/*special handling of 3D mesh*/
	if ((node_tag == TAG_MPEG4_IndexedFaceSet) && codec->info->config.Use3DMeshCoding) {
		if (gf_bs_read_int(bs, 1)) {
			/*nodeID = 1 + */gf_bs_read_int(bs, codec->info->config.NodeIDBits);
			if (codec->UseName) gf_bifs_dec_name(bs, name, 1000);
		}
		/*parse the 3DMesh node*/
		return NULL;
	}
	/*unknown node*/
	if (!node_tag && !proto) {
		codec->LastError = GF_SG_UNKNOWN_NODE;
		return NULL;
	}


	/*DEF'd flag*/
	if (gf_bs_read_int(bs, 1)) {
		if (!codec->info->config.NodeIDBits) {
			codec->LastError = GF_NON_COMPLIANT_BITSTREAM;
			return NULL;
		}
		nodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		if (codec->UseName) gf_bifs_dec_name(bs, name, 1000);
	}

	new_node = NULL;
	skip_init = GF_FALSE;

	/*don't check node IDs duplicate since VRML may use them...*/
#if 0
	/*if a node with same DEF is already in the scene, use it
	we don't do that in memory mode because commands may force replacement
	of a node with a new node with same ID, and we want to be able to dump it (otherwise we would
	dump a USE)*/
	if (nodeID && !codec->dec_memory_mode) {
		new_node = gf_sg_find_node(codec->current_graph, nodeID);
		if (new_node) {
			if (proto) {
				if ((gf_node_get_tag(new_node) != TAG_ProtoNode) || (gf_node_get_proto(new_node) != proto)) {
					codec->LastError = GF_NON_COMPLIANT_BITSTREAM;
					return NULL;
				}
				skip_init = 1;
			} else {
				if (gf_node_get_tag(new_node) != node_tag) {
					codec->LastError = GF_NON_COMPLIANT_BITSTREAM;
					return NULL;
				}
				skip_init = 1;
			}
		}
	}
	if (!new_node)
#endif

	{
		if (proto) {
			/*create proto interface*/
			new_node = gf_sg_proto_create_instance(codec->current_graph, proto);
            //don't init protos unless externProto (in which case we want init for hardcoded protos)
            if (! proto->ExternProto.count) skip_init = GF_TRUE;
		} else {
			new_node = gf_node_new(codec->current_graph, node_tag);
		}
	}
	if (!new_node) {
		codec->LastError = GF_NOT_SUPPORTED;
		return NULL;
	}

	/*update default time fields except in proto parsing*/
	if (!codec->pCurrentProto) UpdateTimeNode(codec, new_node);
	/*nodes are only init outside protos, nodes internal to protos are never intialized */
	else skip_init = GF_TRUE;

	/*if coords were not stored for QP14 before coding this node, reset QP14 it when leaving*/
	reset_qp14 = !codec->coord_stored;

	/*QP 14 is a special quant mode for IndexFace/Line(2D)Set to quantize the
	coordonate(2D) child, based on the first field parsed
	we must check the type of the node and notfy the QP*/
	switch (node_tag) {
	case TAG_MPEG4_Coordinate:
	case TAG_MPEG4_Coordinate2D:
		gf_bifs_dec_qp14_enter(codec, GF_TRUE);
	}

	if (gf_bs_read_int(bs, 1)) {
		e = gf_bifs_dec_node_mask(codec, bs, new_node, proto ? GF_TRUE : GF_FALSE);
	} else {
		e = gf_bifs_dec_node_list(codec, bs, new_node, proto ? GF_TRUE : GF_FALSE);
	}
	if (codec->coord_stored && reset_qp14)
		gf_bifs_dec_qp14_reset(codec);

	if (e) {
		codec->LastError = e;
		/*register*/
		gf_node_register(new_node, NULL);
		/*unregister (deletes)*/
		gf_node_unregister(new_node, NULL);
		return NULL;
	}

	/*VRML: "The transformation hierarchy shall be a directed acyclic graph; results are undefined if a node
	in the transformation hierarchy is its own ancestor"
	that's good, because the scene graph can't handle cyclic graphs (destroy will never be called).
	We therefore only register the node once parsed*/
	if (nodeID) {
		if (strlen(name)) {
			gf_node_set_id(new_node, nodeID, name);
		} else {
			gf_node_set_id(new_node, nodeID, NULL);
		}
	}

	if (!skip_init)
		gf_node_init(new_node);

	switch (node_tag) {
	case TAG_MPEG4_Coordinate:
	case TAG_MPEG4_Coordinate2D:
		gf_bifs_dec_qp14_enter(codec, GF_FALSE);
		break;
	case TAG_MPEG4_Script:
		/*load script if in main graph (useless to load in proto declaration)*/
		if (codec->scenegraph == codec->current_graph) {
			gf_sg_script_load(new_node);
		}
		break;
	/*conditionals must be init*/
	case TAG_MPEG4_Conditional:
		SetupConditional(codec, new_node);
		break;
	}

	/*proto is initialized upon the first traversal to have the same behavior as wth BT/XMT loading*/
#if 0
	/*if new node is a proto and we're in the top scene, load proto code*/
	if (proto && (codec->scenegraph == codec->current_graph)) {
		codec->LastError = gf_sg_proto_load_code(new_node);
	}
#endif

	return new_node;
}