int MqttClient_Auth(MqttClient *client, MqttAuth* auth)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (auth->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

        /* Encode the authentication packet */
        rc = MqttEncode_Auth(client->tx_buf, client->tx_buf_len, auth);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d, QoS %d",
            rc, MqttPacket_TypeDesc(MQTT_PACKET_TYPE_AUTH),
            MQTT_PACKET_TYPE_AUTH, 0, 0);
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
            rc = MqttClient_RespList_Add(client, MQTT_PACKET_TYPE_AUTH, 0,
                &auth->pendResp, auth);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend);
            return rc; /* Error locking client */
        }
    #endif

        /* Send authentication packet */
        rc = MqttPacket_Write(client, client->tx_buf, len);
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
        if (rc != len) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &auth->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
            return rc;
        }

        auth->stat = MQTT_MSG_WAIT;
    }

    /* Wait for auth packet */
    rc = MqttClient_WaitType(client, auth, MQTT_PACKET_TYPE_AUTH, 0,
        client->cmd_timeout_ms);
#ifdef WOLFMQTT_NONBLOCK
    if (rc == MQTT_CODE_CONTINUE)
        return rc;
#endif

#ifdef WOLFMQTT_MULTITHREAD
    if (wm_SemLock(&client->lockClient) == 0) {
        MqttClient_RespList_Remove(client, &auth->pendResp);
        wm_SemUnlock(&client->lockClient);
    }
#endif

    /* reset state */
    auth->stat = MQTT_MSG_BEGIN;

    return rc;
}