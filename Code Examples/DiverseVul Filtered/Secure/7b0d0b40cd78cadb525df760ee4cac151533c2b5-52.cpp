static void selinux_skb_owned_by(struct sk_buff *skb, struct sock *sk)
{
	skb_set_owner_w(skb, sk);
}