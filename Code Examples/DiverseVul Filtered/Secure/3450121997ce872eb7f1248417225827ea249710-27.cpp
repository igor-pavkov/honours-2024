static int snd_line6_control_playback_put(struct snd_kcontrol *kcontrol,
					  struct snd_ctl_elem_value *ucontrol)
{
	int i, changed = 0;
	struct snd_line6_pcm *line6pcm = snd_kcontrol_chip(kcontrol);

	for (i = 0; i < 2; i++)
		if (line6pcm->volume_playback[i] !=
		    ucontrol->value.integer.value[i]) {
			line6pcm->volume_playback[i] =
			    ucontrol->value.integer.value[i];
			changed = 1;
		}

	return changed;
}