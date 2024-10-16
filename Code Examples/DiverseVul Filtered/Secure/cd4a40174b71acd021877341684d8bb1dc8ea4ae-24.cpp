char *copy_mount_string(const void __user *data)
{
	return data ? strndup_user(data, PAGE_SIZE) : NULL;
}