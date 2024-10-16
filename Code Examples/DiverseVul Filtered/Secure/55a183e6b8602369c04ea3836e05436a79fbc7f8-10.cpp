void BD_CheckSFTimeOffset(GF_BifsDecoder *codec, GF_Node *node, GF_FieldInfo *inf)
{
	if (gf_node_get_tag(node) != TAG_ProtoNode) {
		if (!stricmp(inf->name, "startTime") || !stricmp(inf->name, "stopTime"))
			BD_OffsetSFTime(codec,  (Double *)inf->far_ptr);
	} else if (gf_sg_proto_field_is_sftime_offset(node, inf)) {
		BD_OffsetSFTime(codec,  (Double *)inf->far_ptr);
	}
}