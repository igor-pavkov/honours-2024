static void vhost_vdpa_va_unmap(struct vhost_vdpa *v, u64 start, u64 last)
{
	struct vhost_dev *dev = &v->vdev;
	struct vhost_iotlb *iotlb = dev->iotlb;
	struct vhost_iotlb_map *map;
	struct vdpa_map_file *map_file;

	while ((map = vhost_iotlb_itree_first(iotlb, start, last)) != NULL) {
		map_file = (struct vdpa_map_file *)map->opaque;
		fput(map_file->file);
		kfree(map_file);
		vhost_iotlb_map_free(iotlb, map);
	}
}