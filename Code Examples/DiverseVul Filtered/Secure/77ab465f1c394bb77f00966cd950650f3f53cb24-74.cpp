static void js_pushrune(js_State *J, Rune rune)
{
	char buf[UTFmax + 1];
	if (rune > 0) {
		buf[runetochar(buf, &rune)] = 0;
		js_pushstring(J, buf);
	} else {
		js_pushundefined(J);
	}
}