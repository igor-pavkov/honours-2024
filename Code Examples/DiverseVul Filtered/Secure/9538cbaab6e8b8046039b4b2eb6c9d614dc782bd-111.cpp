static void b43_op_tx(struct ieee80211_hw *hw,
		      struct ieee80211_tx_control *control,
		      struct sk_buff *skb)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);

	if (unlikely(skb->len < 2 + 2 + 6)) {
		/* Too short, this can't be a valid frame. */
		ieee80211_free_txskb(hw, skb);
		return;
	}
	B43_WARN_ON(skb_shinfo(skb)->nr_frags);

	skb_queue_tail(&wl->tx_queue[skb->queue_mapping], skb);
	if (!wl->tx_queue_stopped[skb->queue_mapping]) {
		ieee80211_queue_work(wl->hw, &wl->tx_work);
	} else {
		ieee80211_stop_queue(wl->hw, skb->queue_mapping);
	}
}