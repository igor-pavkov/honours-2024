int hid_parse_report(struct hid_device *hid, __u8 *start, unsigned size)
{
	hid->dev_rdesc = kmemdup(start, size, GFP_KERNEL);
	if (!hid->dev_rdesc)
		return -ENOMEM;
	hid->dev_rsize = size;
	return 0;
}