void PrintNode(const char *name, u32 graph_type)
{
#ifdef GPAC_DISABLE_VRML
	fprintf(stderr, "VRML/MPEG-4/X3D scene graph is disabled in this build of GPAC\n");
	return;
#else
	const char *std_name;
	GF_Node *node;
	GF_SceneGraph *sg;
	u32 tag;
#ifndef GPAC_DISABLE_BIFS
#endif /*GPAC_DISABLE_BIFS*/
	Bool is_nodefield = 0;

	char *sep = strchr(name, '.');
	if (sep) {
		sep[0] = 0;
		is_nodefield = 1;
	}

	if (graph_type==1) {
#ifndef GPAC_DISABLE_X3D
		tag = gf_node_x3d_type_by_class_name(name);
		std_name = "X3D";
#else
		fprintf(stderr, "X3D node printing is not supported (X3D support disabled)\n");
		return;
#endif
	} else {
		tag = gf_node_mpeg4_type_by_class_name(name);
		std_name = "MPEG4";
	}
	if (!tag) {
		fprintf(stderr, "Unknown %s node %s\n", std_name, name);
		return;
	}

	sg = gf_sg_new();
	node = gf_node_new(sg, tag);
	gf_node_register(node, NULL);
	name = gf_node_get_class_name(node);
	if (!node) {
		fprintf(stderr, "Node %s not supported in current built\n", name);
		return;
	}
	do_print_node(node, sg, name, graph_type, is_nodefield, GF_FALSE);

	gf_node_unregister(node, NULL);
	gf_sg_del(sg);
#endif /*GPAC_DISABLE_VRML*/
}