void SFCommandBufferChanged(GF_BifsDecoder * codec, GF_Node *node)
{
	void Conditional_BufferReplaced(GF_BifsDecoder * codec, GF_Node *node);

	switch (gf_node_get_tag(node)) {
	case TAG_MPEG4_Conditional:
		Conditional_BufferReplaced(codec, node);
		break;
	}
}