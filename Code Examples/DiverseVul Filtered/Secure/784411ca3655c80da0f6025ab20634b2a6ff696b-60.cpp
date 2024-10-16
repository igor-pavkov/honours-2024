static shape_desc *user_shape(char *name)
{
    int i;
    shape_desc *p;

    if ((p = find_user_shape(name)))
	return p;
    i = N_UserShape++;
    UserShape = ALLOC(N_UserShape, UserShape, shape_desc *);
    p = UserShape[i] = NEW(shape_desc);
    *p = Shapes[0];
    p->name = strdup(name);
    if (Lib == NULL && !streq(name, "custom")) {
	agerr(AGWARN, "using %s for unknown shape %s\n", Shapes[0].name,
	      p->name);
	p->usershape = FALSE;
    } else {
	p->usershape = TRUE;
    }
    return p;
}