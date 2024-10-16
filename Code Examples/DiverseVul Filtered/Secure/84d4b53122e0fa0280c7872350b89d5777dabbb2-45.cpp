int SN_Client_Connect(MqttClient *client, SN_Connect *mc_connect)
{
    int rc = 0, len = 0;
    static byte will_done;

    /* Validate required arguments */
    if ((client == NULL) || (mc_connect == NULL)) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (mc_connect->stat == MQTT_MSG_BEGIN) {

        will_done = 0;

    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

    /* Encode the connect packet */
        rc = SN_Encode_Connect(client->tx_buf, client->tx_buf_len, mc_connect);
#ifdef WOLFMQTT_DEBUG_CLIENT
    PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d, QoS %d",
        rc, SN_Packet_TypeDesc(SN_MSG_TYPE_CONNECT),
        SN_MSG_TYPE_CONNECT, 0, 0);
#endif
        if (rc <= 0) {
        #ifdef WOLFMQTT_MULTITHREAD
            wm_SemUnlock(&client->lockSend);
        #endif
            return rc;
        }
        len = rc;

    #ifdef WOLFMQTT_MULTITHREAD
        rc = wm_SemLock(&client->lockClient);
        if (rc == 0) {
            /* inform other threads of expected response */
            rc = MqttClient_RespList_Add(client,
                    (MqttPacketType)SN_MSG_TYPE_CONNACK, 0,
                    &mc_connect->pendResp, &mc_connect->ack);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend);
            return rc; /* Error locking client */
        }
    #endif

        /* Send connect packet */
        rc = MqttPacket_Write(client, client->tx_buf, len);
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
        if (rc != len) {
            return rc;
        }

        mc_connect->stat = MQTT_MSG_WAIT;
    }

    if ((mc_connect->enable_lwt == 1) && (will_done != 1)) {
        /* If the will is enabled, then the gateway requests the topic and
           message in separate packets. */
        rc = SN_WillTopic(client, &mc_connect->will);
        if (rc != 0) {
            return rc;
        }

        rc = SN_WillMessage(client, &mc_connect->will);
        if (rc != 0) {
            return rc;
        }
        will_done = 1;
    }

    /* Wait for connect ack packet */
    rc = SN_Client_WaitType(client, &mc_connect->ack,
            SN_MSG_TYPE_CONNACK, 0, client->cmd_timeout_ms);
#ifdef WOLFMQTT_NONBLOCK
    if (rc == MQTT_CODE_CONTINUE)
        return rc;
#endif

#ifdef WOLFMQTT_MULTITHREAD
    if (wm_SemLock(&client->lockClient) == 0) {
        MqttClient_RespList_Remove(client, &mc_connect->pendResp);
        wm_SemUnlock(&client->lockClient);
    }
#endif

    /* reset state */
    mc_connect->stat = MQTT_MSG_BEGIN;

    return rc;
}