int vt_do_kbkeycode_ioctl(int cmd, struct kbkeycode __user *user_kbkc,
								int perm)
{
	struct kbkeycode tmp;
	int kc = 0;

	if (copy_from_user(&tmp, user_kbkc, sizeof(struct kbkeycode)))
		return -EFAULT;
	switch (cmd) {
	case KDGETKEYCODE:
		kc = getkeycode(tmp.scancode);
		if (kc >= 0)
			kc = put_user(kc, &user_kbkc->keycode);
		break;
	case KDSETKEYCODE:
		if (!perm)
			return -EPERM;
		kc = setkeycode(tmp.scancode, tmp.keycode);
		break;
	}
	return kc;
}