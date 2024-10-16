static void UpdateTimeNode(GF_BifsDecoder * codec, GF_Node *node)
{
	switch (gf_node_get_tag(node)) {
	case TAG_MPEG4_AnimationStream:
		BD_OffsetSFTime(codec, & ((M_AnimationStream*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_AnimationStream*)node)->stopTime);
		break;
	case TAG_MPEG4_AudioBuffer:
		BD_OffsetSFTime(codec, & ((M_AudioBuffer*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_AudioBuffer*)node)->stopTime);
		break;
	case TAG_MPEG4_AudioClip:
		BD_OffsetSFTime(codec, & ((M_AudioClip*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_AudioClip*)node)->stopTime);
		break;
	case TAG_MPEG4_AudioSource:
		BD_OffsetSFTime(codec, & ((M_AudioSource*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_AudioSource*)node)->stopTime);
		break;
	case TAG_MPEG4_MovieTexture:
		BD_OffsetSFTime(codec, & ((M_MovieTexture*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_MovieTexture*)node)->stopTime);
		break;
	case TAG_MPEG4_TimeSensor:
		BD_OffsetSFTime(codec, & ((M_TimeSensor*)node)->startTime);
		BD_OffsetSFTime(codec, & ((M_TimeSensor*)node)->stopTime);
		break;
	case TAG_ProtoNode:
	{
		u32 i, nbFields;
		GF_FieldInfo inf;
		nbFields = gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_ALL);
		for (i=0; i<nbFields; i++) {
			gf_node_get_field(node, i, &inf);
			if (inf.fieldType != GF_SG_VRML_SFTIME) continue;
			BD_CheckSFTimeOffset(codec, node, &inf);
		}
	}
	break;
	}
}