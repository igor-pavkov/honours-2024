static const struct hid_device_id *hid_match_device(struct hid_device *hdev,
		struct hid_driver *hdrv)
{
	struct hid_dynid *dynid;

	spin_lock(&hdrv->dyn_lock);
	list_for_each_entry(dynid, &hdrv->dyn_list, list) {
		if (hid_match_one_id(hdev, &dynid->id)) {
			spin_unlock(&hdrv->dyn_lock);
			return &dynid->id;
		}
	}
	spin_unlock(&hdrv->dyn_lock);

	return hid_match_id(hdev, hdrv->id_table);
}