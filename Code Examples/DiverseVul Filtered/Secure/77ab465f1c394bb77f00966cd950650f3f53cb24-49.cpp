void js_trap(js_State *J, int pc)
{
	if (pc > 0) {
		js_Function *F = STACK[BOT-1].u.object->u.f.function;
		printf("trap at %d in function ", pc);
		jsC_dumpfunction(J, F);
	}
	jsR_dumpstack(J);
	jsR_dumpenvironment(J, J->E, 0);
	js_stacktrace(J);
}