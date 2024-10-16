static void cfunbody(JF, js_Ast *name, js_Ast *params, js_Ast *body)
{
	F->lightweight = 1;
	F->arguments = 0;

	if (F->script)
		F->lightweight = 0;

	if (body)
		analyze(J, F, body);

	cparams(J, F, params);

	if (name) {
		emit(J, F, OP_CURRENT);
		if (F->lightweight) {
			addlocal(J, F, name, 0);
			emit(J, F, OP_INITLOCAL);
			emitraw(J, F, findlocal(J, F, name->string));
		} else {
			emitstring(J, F, OP_INITVAR, name->string);
		}
	}

	if (body) {
		cvardecs(J, F, body);
		cfundecs(J, F, body);
	}

	if (F->script) {
		emit(J, F, OP_UNDEF);
		cstmlist(J, F, body);
		emit(J, F, OP_RETURN);
	} else {
		cstmlist(J, F, body);
		emit(J, F, OP_UNDEF);
		emit(J, F, OP_RETURN);
	}
}