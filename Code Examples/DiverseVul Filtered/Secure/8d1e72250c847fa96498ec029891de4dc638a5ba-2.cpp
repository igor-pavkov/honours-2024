static ssize_t bcm_char_read(struct file *filp, char __user *buf, size_t size,
			     loff_t *f_pos)
{
	struct bcm_tarang_data *pTarang = filp->private_data;
	struct bcm_mini_adapter *Adapter = pTarang->Adapter;
	struct sk_buff *Packet = NULL;
	ssize_t PktLen = 0;
	int wait_ret_val = 0;
	unsigned long ret = 0;

	wait_ret_val = wait_event_interruptible(Adapter->process_read_wait_queue,
						(pTarang->RxAppControlHead ||
						 Adapter->device_removed));
	if ((wait_ret_val == -ERESTARTSYS)) {
		BCM_DEBUG_PRINT(Adapter, DBG_TYPE_OTHERS, OSAL_DBG, DBG_LVL_ALL,
				"Exiting as i've been asked to exit!!!\n");
		return wait_ret_val;
	}

	if (Adapter->device_removed) {
		BCM_DEBUG_PRINT(Adapter, DBG_TYPE_OTHERS, OSAL_DBG, DBG_LVL_ALL,
				"Device Removed... Killing the Apps...\n");
		return -ENODEV;
	}

	if (FALSE == Adapter->fw_download_done)
		return -EACCES;

	down(&Adapter->RxAppControlQueuelock);

	if (pTarang->RxAppControlHead) {
		Packet = pTarang->RxAppControlHead;
		DEQUEUEPACKET(pTarang->RxAppControlHead,
			      pTarang->RxAppControlTail);
		pTarang->AppCtrlQueueLen--;
	}

	up(&Adapter->RxAppControlQueuelock);

	if (Packet) {
		PktLen = Packet->len;
		ret = copy_to_user(buf, Packet->data,
				   min_t(size_t, PktLen, size));
		if (ret) {
			dev_kfree_skb(Packet);
			BCM_DEBUG_PRINT(Adapter, DBG_TYPE_PRINTK, 0, 0,
					"Returning from copy to user failure\n");
			return -EFAULT;
		}
		BCM_DEBUG_PRINT(Adapter, DBG_TYPE_OTHERS, OSAL_DBG, DBG_LVL_ALL,
				"Read %zd Bytes From Adapter packet = %p by process %d!\n",
				PktLen, Packet, current->pid);
		dev_kfree_skb(Packet);
	}

	BCM_DEBUG_PRINT(Adapter, DBG_TYPE_OTHERS, OSAL_DBG, DBG_LVL_ALL, "<\n");
	return PktLen;
}