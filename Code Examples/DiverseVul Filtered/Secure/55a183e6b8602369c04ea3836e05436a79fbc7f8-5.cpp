GF_Err BD_SetProtoISed(GF_BifsDecoder * codec, u32 protofield, GF_Node *n, u32 nodefield)
{
	/*take care of conditional execution in proto*/
	if (codec->current_graph->pOwningProto) {
		return gf_sg_proto_instance_set_ised((GF_Node *) codec->current_graph->pOwningProto, protofield, n, nodefield);
	}
	/*regular ISed fields*/
	else {
		return gf_sg_proto_field_set_ised(codec->pCurrentProto, protofield, n, nodefield);
	}
}