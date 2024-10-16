static void caif_ctrl_cb(struct cflayer *layr,
				enum caif_ctrlcmd flow,
				int phyid)
{
	struct caifsock *cf_sk = container_of(layr, struct caifsock, layer);
	switch (flow) {
	case CAIF_CTRLCMD_FLOW_ON_IND:
		/* OK from modem to start sending again */
		set_tx_flow_on(cf_sk);
		cf_sk->sk.sk_state_change(&cf_sk->sk);
		break;

	case CAIF_CTRLCMD_FLOW_OFF_IND:
		/* Modem asks us to shut up */
		set_tx_flow_off(cf_sk);
		cf_sk->sk.sk_state_change(&cf_sk->sk);
		break;

	case CAIF_CTRLCMD_INIT_RSP:
		/* We're now connected */
		caif_client_register_refcnt(&cf_sk->layer,
						cfsk_hold, cfsk_put);
		cf_sk->sk.sk_state = CAIF_CONNECTED;
		set_tx_flow_on(cf_sk);
		cf_sk->sk.sk_shutdown = 0;
		cf_sk->sk.sk_state_change(&cf_sk->sk);
		break;

	case CAIF_CTRLCMD_DEINIT_RSP:
		/* We're now disconnected */
		cf_sk->sk.sk_state = CAIF_DISCONNECTED;
		cf_sk->sk.sk_state_change(&cf_sk->sk);
		break;

	case CAIF_CTRLCMD_INIT_FAIL_RSP:
		/* Connect request failed */
		cf_sk->sk.sk_err = ECONNREFUSED;
		cf_sk->sk.sk_state = CAIF_DISCONNECTED;
		cf_sk->sk.sk_shutdown = SHUTDOWN_MASK;
		/*
		 * Socket "standards" seems to require POLLOUT to
		 * be set at connect failure.
		 */
		set_tx_flow_on(cf_sk);
		cf_sk->sk.sk_state_change(&cf_sk->sk);
		break;

	case CAIF_CTRLCMD_REMOTE_SHUTDOWN_IND:
		/* Modem has closed this connection, or device is down. */
		cf_sk->sk.sk_shutdown = SHUTDOWN_MASK;
		cf_sk->sk.sk_err = ECONNRESET;
		set_rx_flow_on(cf_sk);
		cf_sk->sk.sk_error_report(&cf_sk->sk);
		break;

	default:
		pr_debug("Unexpected flow command %d\n", flow);
	}
}