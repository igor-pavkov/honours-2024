const struct hid_device_id *hid_match_id(struct hid_device *hdev,
		const struct hid_device_id *id)
{
	for (; id->bus; id++)
		if (hid_match_one_id(hdev, id))
			return id;

	return NULL;
}