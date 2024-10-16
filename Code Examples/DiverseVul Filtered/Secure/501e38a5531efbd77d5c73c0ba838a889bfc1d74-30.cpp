static void put_ep (struct ep_data *data)
{
	if (likely (!refcount_dec_and_test (&data->count)))
		return;
	put_dev (data->dev);
	/* needs no more cleanup */
	BUG_ON (!list_empty (&data->epfiles));
	BUG_ON (waitqueue_active (&data->wait));
	kfree (data);
}