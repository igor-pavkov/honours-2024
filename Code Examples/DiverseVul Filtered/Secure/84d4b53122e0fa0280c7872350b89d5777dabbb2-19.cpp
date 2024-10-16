int MqttClient_Connect(MqttClient *client, MqttConnect *mc_connect)
{
    int rc, len = 0;

    /* Validate required arguments */
    if (client == NULL || mc_connect == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (mc_connect->stat == MQTT_MSG_BEGIN) {
    #ifdef WOLFMQTT_MULTITHREAD
        /* Lock send socket mutex */
        rc = wm_SemLock(&client->lockSend);
        if (rc != 0) {
            return rc;
        }
    #endif

    #ifdef WOLFMQTT_V5
        /* Use specified protocol version if set */
        mc_connect->protocol_level = client->protocol_level;
    #endif

        /* Encode the connect packet */
        rc = MqttEncode_Connect(client->tx_buf, client->tx_buf_len, mc_connect);
    #ifdef WOLFMQTT_DEBUG_CLIENT
        PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d, QoS %d",
            rc, MqttPacket_TypeDesc(MQTT_PACKET_TYPE_CONNECT),
            MQTT_PACKET_TYPE_CONNECT, 0, 0);
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
            rc = MqttClient_RespList_Add(client, MQTT_PACKET_TYPE_CONNECT_ACK,
                    0, &mc_connect->pendResp, &mc_connect->ack);
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
    #ifdef WOLFMQTT_MULTITHREAD
            if ((rc != MQTT_CODE_CONTINUE) &&
                (wm_SemLock(&client->lockClient)) == 0) {
                MqttClient_RespList_Remove(client, &mc_connect->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
    #endif
            return rc;
        }
    #ifdef WOLFMQTT_V5
        /* Enhanced authentication */
        if (client->enable_eauth == 1) {
            mc_connect->stat = MQTT_MSG_AUTH;
        }
        else
    #endif
        {
            mc_connect->stat = MQTT_MSG_WAIT;
        }
    }

#ifdef WOLFMQTT_V5
    /* Enhanced authentication */
    if (mc_connect->protocol_level > MQTT_CONNECT_PROTOCOL_LEVEL_4 && 
            mc_connect->stat == MQTT_MSG_AUTH)
    {
        MqttAuth auth, *p_auth = &auth;
        MqttProp* prop, *conn_prop;

        /* Find the AUTH property in the connect structure */
        for (conn_prop = mc_connect->props;
             (conn_prop != NULL) && (conn_prop->type != MQTT_PROP_AUTH_METHOD);
             conn_prop = conn_prop->next) {
        }
        if (conn_prop == NULL) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &mc_connect->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
            /* AUTH property was not set in connect structure */
            return MQTT_CODE_ERROR_BAD_ARG;
        }

        XMEMSET((void*)p_auth, 0, sizeof(MqttAuth));

        /* Set the authentication reason */
        p_auth->reason_code = MQTT_REASON_CONT_AUTH;

        /* Use the same authentication method property from connect */
        prop = MqttProps_Add(&p_auth->props);
        prop->type = MQTT_PROP_AUTH_METHOD;
        prop->data_str.str = conn_prop->data_str.str;
        prop->data_str.len = conn_prop->data_str.len;

        /* Send the AUTH packet */
        rc = MqttClient_Auth(client, p_auth);
        MqttClient_PropsFree(p_auth->props);
    #ifdef WOLFMQTT_NONBLOCK
        if (rc == MQTT_CODE_CONTINUE)
            return rc;
    #endif
        if (rc != len) {
        #ifdef WOLFMQTT_MULTITHREAD
            if (wm_SemLock(&client->lockClient) == 0) {
                MqttClient_RespList_Remove(client, &mc_connect->pendResp);
                wm_SemUnlock(&client->lockClient);
            }
        #endif
            return rc;
        }
    }
#endif /* WOLFMQTT_V5 */

    /* Wait for connect ack packet */
    rc = MqttClient_WaitType(client, &mc_connect->ack,
        MQTT_PACKET_TYPE_CONNECT_ACK, 0, client->cmd_timeout_ms);
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