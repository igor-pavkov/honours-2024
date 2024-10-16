shape_desc *bind_shape(char *name, node_t * np)
{
    shape_desc *ptr, *rv = NULL;
    const char *str;

    str = safefile(agget(np, "shapefile"));
    /* If shapefile is defined and not epsf, set shape = custom */
    if (str && !streq(name, "epsf"))
	name = "custom";
    if (!streq(name, "custom")) {
	for (ptr = Shapes; ptr->name; ptr++) {
	    if (streq(ptr->name, name)) {
		rv = ptr;
		break;
	    }
	}
    }
    if (rv == NULL)
	rv = user_shape(name);
    return rv;
}