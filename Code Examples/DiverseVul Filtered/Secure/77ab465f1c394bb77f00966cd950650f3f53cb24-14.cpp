void js_currentfunction(js_State *J)
{
	CHECKSTACK(1);
	STACK[TOP] = STACK[BOT-1];
	++TOP;
}