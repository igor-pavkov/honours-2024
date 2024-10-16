static int getkeycode_helper(struct input_handle *handle, void *data)
{
	struct getset_keycode_data *d = data;

	d->error = input_get_keycode(handle->dev, &d->ke);

	return d->error == 0; /* stop as soon as we successfully get one */
}