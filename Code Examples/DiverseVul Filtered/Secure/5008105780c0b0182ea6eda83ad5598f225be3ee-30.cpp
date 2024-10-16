static void cswitch(JF, js_Ast *ref, js_Ast *head)
{
	js_Ast *node, *clause, *def = NULL;
	int end;

	cexp(J, F, ref);

	/* emit an if-else chain of tests for the case clause expressions */
	for (node = head; node; node = node->b) {
		clause = node->a;
		if (clause->type == STM_DEFAULT) {
			if (def)
				jsC_error(J, clause, "more than one default label in switch");
			def = clause;
		} else {
			cexp(J, F, clause->a);
			clause->casejump = emitjump(J, F, OP_JCASE);
		}
	}
	emit(J, F, OP_POP);
	if (def) {
		def->casejump = emitjump(J, F, OP_JUMP);
		end = 0;
	} else {
		end = emitjump(J, F, OP_JUMP);
	}

	/* emit the casue clause bodies */
	for (node = head; node; node = node->b) {
		clause = node->a;
		label(J, F, clause->casejump);
		if (clause->type == STM_DEFAULT)
			cstmlist(J, F, clause->a);
		else
			cstmlist(J, F, clause->b);
	}

	if (end)
		label(J, F, end);
}