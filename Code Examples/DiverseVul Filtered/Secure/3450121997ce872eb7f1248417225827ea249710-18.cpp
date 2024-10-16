static int snd_line6_impulse_period_put(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_line6_pcm *line6pcm = snd_kcontrol_chip(kcontrol);
	int value = ucontrol->value.integer.value[0];

	if (line6pcm->impulse_period == value)
		return 0;

	line6pcm->impulse_period = value;
	return 1;
}