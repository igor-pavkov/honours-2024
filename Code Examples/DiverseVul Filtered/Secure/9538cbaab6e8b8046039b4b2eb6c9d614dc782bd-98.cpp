static void b43_tx_work(struct work_struct *work)
{
	struct b43_wl *wl = container_of(work, struct b43_wl, tx_work);
	struct b43_wldev *dev;
	struct sk_buff *skb;
	int queue_num;
	int err = 0;

	mutex_lock(&wl->mutex);
	dev = wl->current_dev;
	if (unlikely(!dev || b43_status(dev) < B43_STAT_STARTED)) {
		mutex_unlock(&wl->mutex);
		return;
	}

	for (queue_num = 0; queue_num < B43_QOS_QUEUE_NUM; queue_num++) {
		while (skb_queue_len(&wl->tx_queue[queue_num])) {
			skb = skb_dequeue(&wl->tx_queue[queue_num]);
			if (b43_using_pio_transfers(dev))
				err = b43_pio_tx(dev, skb);
			else
				err = b43_dma_tx(dev, skb);
			if (err == -ENOSPC) {
				wl->tx_queue_stopped[queue_num] = 1;
				ieee80211_stop_queue(wl->hw, queue_num);
				skb_queue_head(&wl->tx_queue[queue_num], skb);
				break;
			}
			if (unlikely(err))
				ieee80211_free_txskb(wl->hw, skb);
			err = 0;
		}

		if (!err)
			wl->tx_queue_stopped[queue_num] = 0;
	}

#if B43_DEBUG
	dev->tx_count++;
#endif
	mutex_unlock(&wl->mutex);
}