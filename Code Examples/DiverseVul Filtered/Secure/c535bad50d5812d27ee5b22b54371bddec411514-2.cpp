static void BM_SetCommandNode(GF_Command *com, GF_Node *node)
{
	com->node = node;
	gf_node_register(node, NULL);
}