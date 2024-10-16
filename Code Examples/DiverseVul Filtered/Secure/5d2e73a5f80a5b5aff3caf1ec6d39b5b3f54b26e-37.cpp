static int zr364xx_vidioc_s_input(struct file *file, void *priv,
				  unsigned int i)
{
	if (i != 0)
		return -EINVAL;
	return 0;
}