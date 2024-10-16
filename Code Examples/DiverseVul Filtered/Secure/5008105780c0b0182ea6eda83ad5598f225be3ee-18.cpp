static void analyze(JF, js_Ast *node)
{
	if (isfun(node->type)) {
		F->lightweight = 0;
		return; /* don't scan inner functions */
	}

	if (node->type == STM_WITH) {
		F->lightweight = 0;
	}

	if (node->type == STM_TRY && node->c) {
		F->lightweight = 0;
	}

	if (node->type == EXP_IDENTIFIER) {
		if (!strcmp(node->string, "arguments")) {
			F->lightweight = 0;
			F->arguments = 1;
		} else if (!strcmp(node->string, "eval")) {
			/* eval may only be used as a direct function call */
			if (!node->parent || node->parent->type != EXP_CALL || node->parent->a != node)
				js_evalerror(J, "%s:%d: invalid use of 'eval'", J->filename, node->line);
			F->lightweight = 0;
		}
	}

	if (node->a) analyze(J, F, node->a);
	if (node->b) analyze(J, F, node->b);
	if (node->c) analyze(J, F, node->c);
	if (node->d) analyze(J, F, node->d);
}