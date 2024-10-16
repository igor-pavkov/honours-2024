static int zr364xx_vidioc_enum_input(struct file *file, void *priv,
				     struct v4l2_input *i)
{
	if (i->index != 0)
		return -EINVAL;
	strscpy(i->name, DRIVER_DESC " Camera", sizeof(i->name));
	i->type = V4L2_INPUT_TYPE_CAMERA;
	return 0;
}