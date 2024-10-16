static js_Ast *returntarget(JF, js_Ast *node)
{
	while (node) {
		if (isfun(node->type))
			return node;
		node = node->parent;
	}
	return NULL;
}