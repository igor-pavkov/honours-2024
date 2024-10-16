static js_Ast *continuetarget(JF, js_Ast *node, const char *label)
{
	while (node) {
		if (isfun(node->type))
			break;
		if (isloop(node->type)) {
			if (!label)
				return node;
			else if (matchlabel(node->parent, label))
				return node;
		}
		node = node->parent;
	}
	return NULL;
}