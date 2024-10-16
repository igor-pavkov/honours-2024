static js_Value *stackidx(js_State *J, int idx)
{
	static js_Value undefined = { {0}, {0}, JS_TUNDEFINED };
	idx = idx < 0 ? TOP + idx : BOT + idx;
	if (idx < 0 || idx >= TOP)
		return &undefined;
	return STACK + idx;
}