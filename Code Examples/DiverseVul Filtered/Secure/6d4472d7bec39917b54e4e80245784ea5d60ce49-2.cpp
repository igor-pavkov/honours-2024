static int hiddev_release(struct inode * inode, struct file * file)
{
	struct hiddev_list *list = file->private_data;
	unsigned long flags;

	spin_lock_irqsave(&list->hiddev->list_lock, flags);
	list_del(&list->node);
	spin_unlock_irqrestore(&list->hiddev->list_lock, flags);

	mutex_lock(&list->hiddev->existancelock);
	if (!--list->hiddev->open) {
		if (list->hiddev->exist) {
			hid_hw_close(list->hiddev->hid);
			hid_hw_power(list->hiddev->hid, PM_HINT_NORMAL);
		} else {
			mutex_unlock(&list->hiddev->existancelock);
			kfree(list->hiddev);
			vfree(list);
			return 0;
		}
	}

	mutex_unlock(&list->hiddev->existancelock);
	vfree(list);

	return 0;
}