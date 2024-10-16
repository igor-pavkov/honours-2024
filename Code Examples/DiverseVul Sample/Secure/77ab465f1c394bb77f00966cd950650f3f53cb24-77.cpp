static void jsR_savescope(js_State *J, js_Environment *newE)
{
	if (J->envtop + 1 >= JS_ENVLIMIT)
		js_stackoverflow(J);
	J->envstack[J->envtop++] = J->E;
	J->E = newE;
}