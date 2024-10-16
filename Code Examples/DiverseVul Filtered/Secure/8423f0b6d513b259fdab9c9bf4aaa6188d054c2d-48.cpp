static int snd_pcm_oss_nonblock(struct file * file)
{
	spin_lock(&file->f_lock);
	file->f_flags |= O_NONBLOCK;
	spin_unlock(&file->f_lock);
	return 0;
}