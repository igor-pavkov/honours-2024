static int hns_roce_add_gid(struct ib_device *device, u8 port_num,
			    unsigned int index, const union ib_gid *gid,
			    const struct ib_gid_attr *attr, void **context)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(device);
	u8 port = port_num - 1;
	unsigned long flags;
	int ret;

	if (port >= hr_dev->caps.num_ports)
		return -EINVAL;

	spin_lock_irqsave(&hr_dev->iboe.lock, flags);

	ret = hr_dev->hw->set_gid(hr_dev, port, index, (union ib_gid *)gid,
				   attr);

	spin_unlock_irqrestore(&hr_dev->iboe.lock, flags);

	return ret;
}