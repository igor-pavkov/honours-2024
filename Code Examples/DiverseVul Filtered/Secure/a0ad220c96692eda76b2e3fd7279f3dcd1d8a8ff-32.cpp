static void ims_pcu_destroy_buttons(struct ims_pcu *pcu)
{
	struct ims_pcu_buttons *buttons = &pcu->buttons;

	input_unregister_device(buttons->input);
}