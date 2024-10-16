void *js_savetry(js_State *J)
{
	if (J->trytop == JS_TRYLIMIT)
		js_error(J, "try: exception stack overflow");
	J->trybuf[J->trytop].E = J->E;
	J->trybuf[J->trytop].envtop = J->envtop;
	J->trybuf[J->trytop].tracetop = J->tracetop;
	J->trybuf[J->trytop].top = J->top;
	J->trybuf[J->trytop].bot = J->bot;
	J->trybuf[J->trytop].pc = NULL;
	return J->trybuf[J->trytop++].buf;
}