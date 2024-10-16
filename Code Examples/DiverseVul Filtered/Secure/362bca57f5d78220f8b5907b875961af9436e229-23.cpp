static void snd_pcm_proc_info_read(struct snd_pcm_substream *substream,
				   struct snd_info_buffer *buffer)
{
	struct snd_pcm_info *info;
	int err;

	if (! substream)
		return;

	info = kmalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return;

	err = snd_pcm_info(substream, info);
	if (err < 0) {
		snd_iprintf(buffer, "error %d\n", err);
		kfree(info);
		return;
	}
	snd_iprintf(buffer, "card: %d\n", info->card);
	snd_iprintf(buffer, "device: %d\n", info->device);
	snd_iprintf(buffer, "subdevice: %d\n", info->subdevice);
	snd_iprintf(buffer, "stream: %s\n", snd_pcm_stream_name(info->stream));
	snd_iprintf(buffer, "id: %s\n", info->id);
	snd_iprintf(buffer, "name: %s\n", info->name);
	snd_iprintf(buffer, "subname: %s\n", info->subname);
	snd_iprintf(buffer, "class: %d\n", info->dev_class);
	snd_iprintf(buffer, "subclass: %d\n", info->dev_subclass);
	snd_iprintf(buffer, "subdevices_count: %d\n", info->subdevices_count);
	snd_iprintf(buffer, "subdevices_avail: %d\n", info->subdevices_avail);
	kfree(info);
}