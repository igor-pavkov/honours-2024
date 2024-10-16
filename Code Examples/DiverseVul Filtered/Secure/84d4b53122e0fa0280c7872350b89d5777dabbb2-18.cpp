int SN_Client_Register(MqttClient *client, SN_Register *regist)
{
    int rc, len;

    /* Validate required arguments */
    if (client == NULL || regist == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (regist->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

        /* Encode the register packet */
        rc = SN_Encode_Register(client->tx_buf, client->tx_buf_len, regist);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d)",
            rc, SN_Packet_TypeDesc(SN_MSG_TYPE_REGISTER),
            SN_MSG_TYPE_REGISTER);
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
                    (MqttPacketType)SN_MSG_TYPE_REGACK,
                    regist->packet_id, &regist->pendResp, &regist->regack);
            wm_SemUnlock(&client->lockClient);
        }
        if (rc != 0) {
            wm_SemUnlock(&client->lockSend);
            return rc; /* Error locking client */
        }
    #endif

        /* Send register packet */
        rc = MqttPacket_Write(client, client->tx_buf, len);
    #ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockSend);
    #endif
        if (rc != len) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &regist->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
            return rc;
        }

        regist->stat = MQTT_MSG_WAIT;
    }

    /* Wait for register acknowledge packet */
    rc = SN_Client_WaitType(client, &regist->regack,
            SN_MSG_TYPE_REGACK, regist->packet_id, client->cmd_timeout_ms);
#ifdef WOLFMQTT_NONBLOCK
    if (rc == MQTT_CODE_CONTINUE)
        return rc;
#endif
#ifdef WOLFMQTT_MULTITHREAD
    if (wm_SemLock(&client->lockClient) == 0) {
        MqttClient_RespList_Remove(client, &regist->pendResp);
        wm_SemUnlock(&client->lockClient);
    }
#endif

    /* reset state */
    regist->stat = MQTT_MSG_BEGIN;

    return rc;
}