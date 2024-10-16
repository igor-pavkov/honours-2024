int MqttClient_Ping_ex(MqttClient *client, MqttPing* ping)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL || ping == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (ping->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

        /* Encode the subscribe packet */
        rc = MqttEncode_Ping(client->tx_buf, client->tx_buf_len, ping);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d, QoS %d",
            rc, MqttPacket_TypeDesc(MQTT_PACKET_TYPE_PING_REQ),
            MQTT_PACKET_TYPE_PING_REQ, 0, 0);
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
            rc = MqttClient_RespList_Add(client, MQTT_PACKET_TYPE_PING_RESP, 0,
                &ping->pendResp, ping);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend);
            return rc; /* Error locking client */
        }
    #endif

        /* Send ping req packet */
        rc = MqttPacket_Write(client, client->tx_buf, len);
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
        if (rc != len) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &ping->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
            return rc;
        }

        ping->stat = MQTT_MSG_WAIT;
    }

    /* Wait for ping resp packet */
    rc = MqttClient_WaitType(client, ping, MQTT_PACKET_TYPE_PING_RESP, 0,
        client->cmd_timeout_ms);
#ifdef WOLFMQTT_NONBLOCK
    if (rc == MQTT_CODE_CONTINUE)
        return rc;
#endif

#ifdef WOLFMQTT_MULTITHREAD
    if (wm_SemLock(&client->lockClient) == 0) {
        MqttClient_RespList_Remove(client, &ping->pendResp);
        wm_SemUnlock(&client->lockClient);
    }
#endif

    /* reset state */
    ping->stat = MQTT_MSG_BEGIN;

    return rc;
}