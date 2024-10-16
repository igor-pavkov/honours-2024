static int MqttClient_Publish_ReadPayload(MqttClient* client,
    MqttPublish* publish, int timeout_ms)
{
    int rc = MQTT_CODE_SUCCESS;
    byte msg_done;

    /* Handle packet callback and read remaining payload */
    do {
        /* Determine if message is done */
        msg_done = ((publish->buffer_pos + publish->buffer_len) >=
                    publish->total_len) ? 1 : 0;

        if (publish->buffer_new) {
            /* Issue callback for new message (first time only) */
            if (client->msg_cb) {
                /* if using the temp publish message buffer,
                   then populate message context with client context */
                if (publish->ctx == NULL && &client->msg.publish == publish) {
                    publish->ctx = client->ctx;
                }
                rc = client->msg_cb(client, publish, publish->buffer_new,
                                    msg_done);
                if (rc != MQTT_CODE_SUCCESS) {
                    return rc;
                };
            }

            /* Reset topic name since valid on new message only */
            publish->topic_name = NULL;
            publish->topic_name_len = 0;

            publish->buffer_new = 0;
        }

        /* Read payload */
        if (!msg_done) {
            int msg_len;

            /* add last length to position and reset len */
            publish->buffer_pos += publish->buffer_len;
            publish->buffer_len = 0;

            /* set state to reading payload */
            publish->stat = MQTT_MSG_READ_PAYLOAD;

            msg_len = (publish->total_len - publish->buffer_pos);
            if (msg_len > client->rx_buf_len) {
                msg_len = client->rx_buf_len;
            }

            /* make sure there is something to read */
            if (msg_len > 0) {
                #ifdef WOLFMQTT_TEST_NONBLOCK
                    if (!testNbAlt) {
                        testNbAlt = 1;
                        return MQTT_CODE_CONTINUE;
                    }
                    testNbAlt = 0;
                #endif

                rc = MqttSocket_Read(client, client->rx_buf, msg_len,
                        timeout_ms);
                if (rc < 0) {
                    break;
                }

                /* Update message */
                publish->buffer = client->rx_buf;
                publish->buffer_len = rc;
                rc = MQTT_CODE_SUCCESS; /* mark success */

                msg_done = ((publish->buffer_pos + publish->buffer_len) >=
                    publish->total_len) ? 1 : 0;

                /* Issue callback for additional publish payload */
                if (client->msg_cb) {
                    rc = client->msg_cb(client, publish, publish->buffer_new,
                                        msg_done);
                    if (rc != MQTT_CODE_SUCCESS) {
                        return rc;
                    };
                }
            }
        }
    } while (!msg_done);

    return rc;
}