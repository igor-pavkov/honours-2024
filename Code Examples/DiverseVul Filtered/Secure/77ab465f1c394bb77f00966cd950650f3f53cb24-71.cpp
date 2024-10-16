static void jsR_pushtrace(js_State *J, const char *name, const char *file, int line)
{
	if (J->tracetop + 1 == JS_ENVLIMIT)
		js_error(J, "call stack overflow");
	++J->tracetop;
	J->trace[J->tracetop].name = name;
	J->trace[J->tracetop].file = file;
	J->trace[J->tracetop].line = line;
}