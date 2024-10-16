static void jsR_restorescope(js_State *J)
{
	J->E = J->envstack[--J->envtop];
}