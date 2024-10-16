static int kd_sound_helper(struct input_handle *handle, void *data)
{
	unsigned int *hz = data;
	struct input_dev *dev = handle->dev;

	if (test_bit(EV_SND, dev->evbit)) {
		if (test_bit(SND_TONE, dev->sndbit)) {
			input_inject_event(handle, EV_SND, SND_TONE, *hz);
			if (*hz)
				return 0;
		}
		if (test_bit(SND_BELL, dev->sndbit))
			input_inject_event(handle, EV_SND, SND_BELL, *hz ? 1 : 0);
	}

	return 0;
}