static void cparams(JF, js_Ast *list)
{
	F->numparams = listlength(list);
	while (list) {
		addlocal(J, F, list->a, 0);
		list = list->b;
	}
}