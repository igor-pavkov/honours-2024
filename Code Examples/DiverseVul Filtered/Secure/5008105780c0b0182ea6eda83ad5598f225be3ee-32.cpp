static void checkdup(JF, js_Ast *list, js_Ast *end)
{
	char nbuf[32], sbuf[32];
	const char *needle, *straw;

	if (end->a->type == EXP_NUMBER)
		needle = jsV_numbertostring(J, nbuf, end->a->number);
	else
		needle = end->a->string;

	while (list->a != end) {
		if (list->a->type == end->type) {
			js_Ast *prop = list->a->a;
			if (prop->type == EXP_NUMBER)
				straw = jsV_numbertostring(J, sbuf, prop->number);
			else
				straw =  prop->string;
			if (!strcmp(needle, straw))
				jsC_error(J, list, "duplicate property '%s' in object literal", needle);
		}
		list = list->b;
	}
}