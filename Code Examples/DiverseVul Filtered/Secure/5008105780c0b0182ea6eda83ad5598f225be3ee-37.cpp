static void cexit(JF, enum js_AstType T, js_Ast *node, js_Ast *target)
{
	js_Ast *prev;
	do {
		prev = node, node = node->parent;
		switch (node->type) {
		default: /* impossible */ break;
		case STM_WITH:
			emit(J, F, OP_ENDWITH);
			break;
		case STM_FOR_IN:
		case STM_FOR_IN_VAR:
			/* pop the iterator if leaving the loop */
			if (F->script) {
				if (T == STM_RETURN || T == STM_BREAK || (T == STM_CONTINUE && target != node)) {
					/* pop the iterator, save the return or exp value */
					emit(J, F, OP_ROT2);
					emit(J, F, OP_POP);
				}
				if (T == STM_CONTINUE)
					emit(J, F, OP_ROT2); /* put the iterator back on top */
			} else {
				if (T == STM_RETURN) {
					/* pop the iterator, save the return value */
					emit(J, F, OP_ROT2);
					emit(J, F, OP_POP);
				}
				if (T == STM_BREAK || (T == STM_CONTINUE && target != node))
					emit(J, F, OP_POP); /* pop the iterator */
			}
			break;
		case STM_TRY:
			/* came from try block */
			if (prev == node->a) {
				emit(J, F, OP_ENDTRY);
				if (node->d) cstm(J, F, node->d); /* finally */
			}
			/* came from catch block */
			if (prev == node->c) {
				/* ... with finally */
				if (node->d) {
					emit(J, F, OP_ENDCATCH);
					emit(J, F, OP_ENDTRY);
					cstm(J, F, node->d); /* finally */
				} else {
					emit(J, F, OP_ENDCATCH);
				}
			}
			break;
		}
	} while (node != target);
}