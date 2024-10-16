static int SN_WillMessage(MqttClient *client, SN_Will *will)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

#ifdef WOLFMQTT_MULTITHREAD
    rc = wm_SemLock(&client->lockClient);
    if (rc == 0) {
        /* inform other threads of expected response */
        rc = MqttClient_RespList_Add(client,
                (MqttPacketType)SN_MSG_TYPE_WILLMSGREQ, 0,
                &will->pendResp, &will->resp.msgResp);
        wm_SemUnlock(&client->lockClient);
    }
    if (rc != 0) {
        return rc; /* Error locking client */
    }
#endif

    /* Wait for Will Message Request */
    rc = SN_Client_WaitType(client, &will->resp.msgResp,
            SN_MSG_TYPE_WILLMSGREQ, 0, client->cmd_timeout_ms);

#ifdef WOLFMQTT_NONBLOCK
    if (rc == MQTT_CODE_CONTINUE)
        return rc;
#endif

#ifdef WOLFMQTT_MULTITHREAD
    if (wm_SemLock(&client->lockClient) == 0) {
        MqttClient_RespList_Remove(client, &will->pendResp);
        wm_SemUnlock(&client->lockClient);
    }
#endif

    if (rc == 0) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif
        /* Encode Will Message */
        len = rc = SN_Encode_WillMsg(client->tx_buf,
            client->tx_buf_len, will);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("EncodePacket: Len %d, Type %s (%d)",
            rc, SN_Packet_TypeDesc(SN_MSG_TYPE_WILLMSG),
            SN_MSG_TYPE_WILLMSG);
    #endif
        if (rc > 0) {
            /* Send Will Topic packet */
            rc = MqttPacket_Write(client, client->tx_buf, len);
            if (rc == len) {
                rc = 0;
            }
        }
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
    }

    return rc;
}