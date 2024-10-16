static void cvardecs(JF, js_Ast *node)
{
	if (isfun(node->type))
		return; /* stop at inner functions */

	if (node->type == EXP_VAR) {
		if (F->lightweight)
			addlocal(J, F, node->a, 1);
		else
			emitstring(J, F, OP_DEFVAR, node->a->string);
	}

	if (node->a) cvardecs(J, F, node->a);
	if (node->b) cvardecs(J, F, node->b);
	if (node->c) cvardecs(J, F, node->c);
	if (node->d) cvardecs(J, F, node->d);
}