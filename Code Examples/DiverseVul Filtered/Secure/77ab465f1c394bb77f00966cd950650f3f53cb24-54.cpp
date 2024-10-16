void js_remove(js_State *J, int idx)
{
	idx = idx < 0 ? TOP + idx : BOT + idx;
	if (idx < BOT || idx >= TOP)
		js_error(J, "stack error!");
	for (;idx < TOP - 1; ++idx)
		STACK[idx] = STACK[idx+1];
	--TOP;
}