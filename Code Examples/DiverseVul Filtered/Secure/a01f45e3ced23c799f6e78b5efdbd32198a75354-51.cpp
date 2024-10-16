static int h2_process_mux(struct h2c *h2c)
{
	struct h2s *h2s, *h2s_back;

	if (unlikely(h2c->st0 < H2_CS_FRAME_H)) {
		if (unlikely(h2c->st0 == H2_CS_PREFACE && (h2c->flags & H2_CF_IS_BACK))) {
			if (unlikely(h2c_bck_send_preface(h2c) <= 0)) {
				/* RFC7540#3.5: a GOAWAY frame MAY be omitted */
				if (h2c->st0 == H2_CS_ERROR) {
					h2c->st0 = H2_CS_ERROR2;
					sess_log(h2c->conn->owner);
				}
				goto fail;
			}
			h2c->st0 = H2_CS_SETTINGS1;
		}
		/* need to wait for the other side */
		if (h2c->st0 < H2_CS_FRAME_H)
			return 1;
	}

	/* start by sending possibly pending window updates */
	if (h2c->rcvd_c > 0 &&
	    !(h2c->flags & (H2_CF_MUX_MFULL | H2_CF_MUX_MALLOC)) &&
	    h2c_send_conn_wu(h2c) < 0)
		goto fail;

	/* First we always process the flow control list because the streams
	 * waiting there were already elected for immediate emission but were
	 * blocked just on this.
	 */

	list_for_each_entry_safe(h2s, h2s_back, &h2c->fctl_list, list) {
		if (h2c->mws <= 0 || h2c->flags & H2_CF_MUX_BLOCK_ANY ||
		    h2c->st0 >= H2_CS_ERROR)
			break;

		h2s->flags &= ~H2_SF_BLK_ANY;
		h2s->send_wait->events &= ~SUB_RETRY_SEND;
		h2s->send_wait->events |= SUB_CALL_UNSUBSCRIBE;
		tasklet_wakeup(h2s->send_wait->task);
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
		LIST_ADDQ(&h2c->sending_list, &h2s->list);
	}

	list_for_each_entry_safe(h2s, h2s_back, &h2c->send_list, list) {
		if (h2c->st0 >= H2_CS_ERROR || h2c->flags & H2_CF_MUX_BLOCK_ANY)
			break;

		h2s->flags &= ~H2_SF_BLK_ANY;
		h2s->send_wait->events &= ~SUB_RETRY_SEND;
		h2s->send_wait->events |= SUB_CALL_UNSUBSCRIBE;
		tasklet_wakeup(h2s->send_wait->task);
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
		LIST_ADDQ(&h2c->sending_list, &h2s->list);
	}

 fail:
	if (unlikely(h2c->st0 >= H2_CS_ERROR)) {
		if (h2c->st0 == H2_CS_ERROR) {
			if (h2c->max_id >= 0) {
				h2c_send_goaway_error(h2c, NULL);
				if (h2c->flags & H2_CF_MUX_BLOCK_ANY)
					return 0;
			}

			h2c->st0 = H2_CS_ERROR2; // sent (or failed hard) !
		}
		return 1;
	}
	return (h2c->mws <= 0 || LIST_ISEMPTY(&h2c->fctl_list)) && LIST_ISEMPTY(&h2c->send_list);
}