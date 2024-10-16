static void emitline(JF, js_Ast *node)
{
	if (F->lastline != node->line) {
		F->lastline = node->line;
		emit(J, F, OP_LINE);
		emitraw(J, F, node->line);
	}
}