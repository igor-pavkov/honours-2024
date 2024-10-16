static void emitlocal(JF, int oploc, int opvar, js_Ast *ident)
{
	int i;
	if (J->strict && oploc == OP_SETLOCAL) {
		if (!strcmp(ident->string, "arguments"))
			jsC_error(J, ident, "'arguments' is read-only in strict mode");
		if (!strcmp(ident->string, "eval"))
			jsC_error(J, ident, "'eval' is read-only in strict mode");
	}
	if (F->lightweight) {
		i = findlocal(J, F, ident->string);
		if (i >= 0) {
			emit(J, F, oploc);
			emitraw(J, F, i);
			return;
		}
	}
	emitstring(J, F, opvar, ident->string);
}