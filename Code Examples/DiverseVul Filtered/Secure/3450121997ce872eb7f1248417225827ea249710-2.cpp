static int snd_line6_impulse_volume_put(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_line6_pcm *line6pcm = snd_kcontrol_chip(kcontrol);
	int value = ucontrol->value.integer.value[0];
	int err;

	if (line6pcm->impulse_volume == value)
		return 0;

	line6pcm->impulse_volume = value;
	if (value > 0) {
		err = line6_pcm_acquire(line6pcm, LINE6_STREAM_IMPULSE, true);
		if (err < 0) {
			line6pcm->impulse_volume = 0;
			return err;
		}
	} else {
		line6_pcm_release(line6pcm, LINE6_STREAM_IMPULSE);
	}
	return 1;
}