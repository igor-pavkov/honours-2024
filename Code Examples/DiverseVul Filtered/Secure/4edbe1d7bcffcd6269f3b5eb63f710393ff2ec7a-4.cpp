static int dev_set_geometry(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	int r = -EINVAL, x;
	struct mapped_device *md;
	struct hd_geometry geometry;
	unsigned long indata[4];
	char *geostr = (char *) param + param->data_start;
	char dummy;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	if (geostr < param->data ||
	    invalid_str(geostr, (void *) param + param_size)) {
		DMWARN("Invalid geometry supplied.");
		goto out;
	}

	x = sscanf(geostr, "%lu %lu %lu %lu%c", indata,
		   indata + 1, indata + 2, indata + 3, &dummy);

	if (x != 4) {
		DMWARN("Unable to interpret geometry settings.");
		goto out;
	}

	if (indata[0] > 65535 || indata[1] > 255 ||
	    indata[2] > 255 || indata[3] > ULONG_MAX) {
		DMWARN("Geometry exceeds range limits.");
		goto out;
	}

	geometry.cylinders = indata[0];
	geometry.heads = indata[1];
	geometry.sectors = indata[2];
	geometry.start = indata[3];

	r = dm_set_geometry(md, &geometry);

	param->data_size = 0;

out:
	dm_put(md);
	return r;
}