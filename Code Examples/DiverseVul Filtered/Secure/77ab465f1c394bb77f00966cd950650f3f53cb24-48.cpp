int js_isnull(js_State *J, int idx) { return stackidx(J, idx)->type == JS_TNULL; }