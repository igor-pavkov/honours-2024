static js_Ast *breaktarget(JF, js_Ast *node, const char *label)
{
	while (node) {
		if (isfun(node->type))
			break;
		if (!label) {
			if (isloop(node->type) || node->type == STM_SWITCH)
				return node;
		} else {
			if (matchlabel(node->parent, label))
				return node;
		}
		node = node->parent;
	}
	return NULL;
}