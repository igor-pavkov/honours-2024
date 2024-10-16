long keyctl_restrict_keyring(key_serial_t id, const char __user *_type,
			     const char __user *_restriction)
{
	key_ref_t key_ref;
	bool link_reject = !_type;
	char type[32];
	char *restriction = NULL;
	long ret;

	key_ref = lookup_user_key(id, 0, KEY_NEED_SETATTR);
	if (IS_ERR(key_ref))
		return PTR_ERR(key_ref);

	if (_type) {
		ret = key_get_type_from_user(type, _type, sizeof(type));
		if (ret < 0)
			goto error;
	}

	if (_restriction) {
		if (!_type) {
			ret = -EINVAL;
			goto error;
		}

		restriction = strndup_user(_restriction, PAGE_SIZE);
		if (IS_ERR(restriction)) {
			ret = PTR_ERR(restriction);
			goto error;
		}
	}

	ret = keyring_restrict(key_ref, link_reject ? NULL : type, restriction);
	kfree(restriction);

error:
	key_ref_put(key_ref);

	return ret;
}