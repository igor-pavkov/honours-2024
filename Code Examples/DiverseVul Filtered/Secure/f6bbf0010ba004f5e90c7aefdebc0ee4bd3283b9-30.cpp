static int perm_to_iommu_flags(u32 perm)
{
	int flags = 0;

	switch (perm) {
	case VHOST_ACCESS_WO:
		flags |= IOMMU_WRITE;
		break;
	case VHOST_ACCESS_RO:
		flags |= IOMMU_READ;
		break;
	case VHOST_ACCESS_RW:
		flags |= (IOMMU_WRITE | IOMMU_READ);
		break;
	default:
		WARN(1, "invalidate vhost IOTLB permission\n");
		break;
	}

	return flags | IOMMU_CACHE;
}