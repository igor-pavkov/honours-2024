int js_isboolean(js_State *J, int idx) { return stackidx(J, idx)->type == JS_TBOOLEAN; }