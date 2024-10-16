static ssize_t pstr_show(struct device *kdev,
			 struct device_attribute *kattr, char *buf)
{
	struct hid_device *hdev = to_hid_device(kdev);
	struct cp2112_pstring_attribute *attr =
		container_of(kattr, struct cp2112_pstring_attribute, attr);
	struct cp2112_string_report report;
	u8 length;
	int ret;

	ret = cp2112_hid_get(hdev, attr->report, &report.report,
			     sizeof(report) - 1, HID_FEATURE_REPORT);
	if (ret < 3) {
		hid_err(hdev, "error reading %s string: %d\n", kattr->attr.name,
			ret);
		if (ret < 0)
			return ret;
		return -EIO;
	}

	if (report.length < 2) {
		hid_err(hdev, "invalid %s string length: %d\n",
			kattr->attr.name, report.length);
		return -EIO;
	}

	length = report.length > ret - 1 ? ret - 1 : report.length;
	length = (length - 2) / sizeof(report.string[0]);
	ret = utf16s_to_utf8s(report.string, length, UTF16_LITTLE_ENDIAN, buf,
			      PAGE_SIZE - 1);
	buf[ret++] = '\n';
	return ret;
}