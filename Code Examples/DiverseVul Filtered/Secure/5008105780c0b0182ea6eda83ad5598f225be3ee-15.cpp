static int listlength(js_Ast *list)
{
	int n = 0;
	while (list) ++n, list = list->b;
	return n;
}