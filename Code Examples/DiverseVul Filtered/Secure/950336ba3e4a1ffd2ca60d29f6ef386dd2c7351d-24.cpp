static void ati_remote2_close(struct input_dev *idev)
{
	struct ati_remote2 *ar2 = input_get_drvdata(idev);

	dev_dbg(&ar2->intf[0]->dev, "%s()\n", __func__);

	mutex_lock(&ati_remote2_mutex);

	if (!(ar2->flags & ATI_REMOTE2_SUSPENDED))
		ati_remote2_kill_urbs(ar2);

	ar2->flags &= ~ATI_REMOTE2_OPENED;

	mutex_unlock(&ati_remote2_mutex);
}