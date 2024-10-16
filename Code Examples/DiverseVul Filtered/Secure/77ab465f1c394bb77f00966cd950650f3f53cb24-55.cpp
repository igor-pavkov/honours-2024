void js_endtry(js_State *J)
{
	if (J->trytop == 0)
		js_error(J, "endtry: exception stack underflow");
	--J->trytop;
}