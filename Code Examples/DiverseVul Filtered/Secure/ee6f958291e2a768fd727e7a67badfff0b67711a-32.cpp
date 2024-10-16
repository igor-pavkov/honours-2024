static int lives_below_in_same_fs(struct dentry *d, struct dentry *dentry)
{
	while (1) {
		if (d == dentry)
			return 1;
		if (d == NULL || d == d->d_parent)
			return 0;
		d = d->d_parent;
	}
}