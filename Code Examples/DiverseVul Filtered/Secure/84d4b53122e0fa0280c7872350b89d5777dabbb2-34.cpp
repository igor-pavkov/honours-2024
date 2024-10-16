int SN_Client_Unsubscribe(MqttClient *client, SN_Unsubscribe *unsubscribe)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL || unsubscribe == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (unsubscribe->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

        /* Encode the subscribe packet */
        rc = SN_Encode_Unsubscribe(client->tx_buf, client->tx_buf_len,
            unsubscribe);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d)",
            rc, SN_Packet_TypeDesc(SN_MSG_TYPE_UNSUBSCRIBE),
            SN_MSG_TYPE_UNSUBSCRIBE);
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
                    (MqttPacketType)SN_MSG_TYPE_UNSUBACK,
                    0, &unsubscribe->pendResp, &unsubscribe->ack);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend);
            return rc; /* Error locking client */
        }
    #endif

        /* Send unsubscribe packet */
        rc = MqttPacket_Write(client, client->tx_buf, len);
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
        if (rc != len) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &unsubscribe->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
        }
        unsubscribe->stat = MQTT_MSG_WAIT;
    }

    /* Wait for unsubscribe ack packet */
    rc = SN_Client_WaitType(client, &unsubscribe->ack,
            SN_MSG_TYPE_UNSUBACK, unsubscribe->packet_id,
            client->cmd_timeout_ms);
    #ifdef WOLFMQTT_NONBLOCK
        if (rc == MQTT_CODE_CONTINUE)
            return rc;
    #endif
    #ifdef WOLFMQTT_MULTITHREAD
        if (wm_SemLock(&client->lockClient) == 0) {
            MqttClient_RespList_Remove(client, &unsubscribe->pendResp);
            wm_SemUnlock(&client->lockClient);
        }
    #endif

    /* reset state */
    unsubscribe->stat = MQTT_MSG_BEGIN;

    return rc;
}