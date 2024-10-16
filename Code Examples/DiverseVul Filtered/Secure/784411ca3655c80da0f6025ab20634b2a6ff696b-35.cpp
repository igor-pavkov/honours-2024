shape_desc *find_user_shape(const char *name)
{
    int i;
    if (UserShape) {
	for (i = 0; i < N_UserShape; i++) {
	    if (streq(UserShape[i]->name, name))
		return UserShape[i];
	}
    }
    return NULL;
}