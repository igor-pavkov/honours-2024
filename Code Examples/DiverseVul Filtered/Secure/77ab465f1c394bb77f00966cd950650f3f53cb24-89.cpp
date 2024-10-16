void js_replace(js_State* J, int idx)
{
	idx = idx < 0 ? TOP + idx : BOT + idx;
	if (idx < BOT || idx >= TOP)
		js_error(J, "stack error!");
	STACK[idx] = STACK[--TOP];
}