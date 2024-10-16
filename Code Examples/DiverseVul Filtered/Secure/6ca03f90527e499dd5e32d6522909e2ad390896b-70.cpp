static bool kbd_match(struct input_handler *handler, struct input_dev *dev)
{
	int i;

	if (test_bit(EV_SND, dev->evbit))
		return true;

	if (test_bit(EV_KEY, dev->evbit)) {
		for (i = KEY_RESERVED; i < BTN_MISC; i++)
			if (test_bit(i, dev->keybit))
				return true;
		for (i = KEY_BRL_DOT1; i <= KEY_BRL_DOT10; i++)
			if (test_bit(i, dev->keybit))
				return true;
	}

	return false;
}