static void kvm_destroy_devices(struct kvm *kvm)
{
	struct list_head *node, *tmp;

	list_for_each_safe(node, tmp, &kvm->devices) {
		struct kvm_device *dev =
			list_entry(node, struct kvm_device, vm_node);

		list_del(node);
		dev->ops->destroy(dev);
	}
}