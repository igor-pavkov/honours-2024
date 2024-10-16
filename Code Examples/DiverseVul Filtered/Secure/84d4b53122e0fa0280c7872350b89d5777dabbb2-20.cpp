int MqttClient_Unsubscribe(MqttClient *client, MqttUnsubscribe *unsubscribe)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL || unsubscribe == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

#ifdef WOLFMQTT_V5
    /* Use specified protocol version if set */
    unsubscribe->protocol_level = client->protocol_level;
#endif

    if (unsubscribe->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

        /* Encode the subscribe packet */
        rc = MqttEncode_Unsubscribe(client->tx_buf, client->tx_buf_len,
            unsubscribe);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d, QoS %d",
            rc, MqttPacket_TypeDesc(MQTT_PACKET_TYPE_UNSUBSCRIBE),
            MQTT_PACKET_TYPE_UNSUBSCRIBE, unsubscribe->packet_id, 0);
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
                MQTT_PACKET_TYPE_UNSUBSCRIBE_ACK, unsubscribe->packet_id,
                &unsubscribe->pendResp, &unsubscribe->ack);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend); /* Error locking client */
            return rc;
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
            return rc;
        }

        unsubscribe->stat = MQTT_MSG_WAIT;
    }

    /* Wait for unsubscribe ack packet */
    rc = MqttClient_WaitType(client, &unsubscribe->ack,
        MQTT_PACKET_TYPE_UNSUBSCRIBE_ACK, unsubscribe->packet_id,
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

#ifdef WOLFMQTT_V5
    if (unsubscribe->ack.props != NULL) {
        /* Release the allocated properties */
        MqttClient_PropsFree(unsubscribe->ack.props);
    }
#endif

    /* reset state */
    unsubscribe->stat = MQTT_MSG_BEGIN;

    return rc;
}