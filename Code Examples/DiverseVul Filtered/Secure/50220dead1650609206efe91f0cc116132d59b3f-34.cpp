void hid_unregister_driver(struct hid_driver *hdrv)
{
	driver_remove_file(&hdrv->driver, &driver_attr_new_id);
	driver_unregister(&hdrv->driver);
	hid_free_dynids(hdrv);
}