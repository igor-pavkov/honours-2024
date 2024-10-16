int SN_Client_Publish(MqttClient *client, SN_Publish *publish)
{
    int rc = MQTT_CODE_SUCCESS;
    SN_MsgType resp_type;

    /* Validate required arguments */
    if (client == NULL || publish == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    switch ((int)publish->stat)
    {
        case MQTT_MSG_BEGIN:
        {
        #ifdef WOLFMQTT_MULTITHREAD
            /* Lock send socket mutex */
            rc = wm_SemLock(&client->lockSend);
            if (rc != 0) {
                return rc;
            }
        #endif

            /* Encode the publish packet */
            rc = SN_Encode_Publish(client->tx_buf, client->tx_buf_len,
                    publish);
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d,"
                    " QoS %d",
                rc, SN_Packet_TypeDesc(SN_MSG_TYPE_PUBLISH),
                SN_MSG_TYPE_PUBLISH, publish->packet_id,
                publish->qos);
        #endif
            if (rc <= 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                wm_SemUnlock(&client->lockSend);
            #endif
                return rc;
            }

            client->write.len = rc;
            publish->buffer_pos = 0;

        #ifdef WOLFMQTT_MULTITHREAD
            if ((publish->qos == MQTT_QOS_1) ||
                (publish->qos == MQTT_QOS_2)) {
                resp_type = (publish->qos == MQTT_QOS_1) ?
                        SN_MSG_TYPE_PUBACK :
                        SN_MSG_TYPE_PUBCOMP;

                rc = wm_SemLock(&client->lockClient);
                if (rc == 0) {
                    /* inform other threads of expected response */
                    rc = MqttClient_RespList_Add(client,
                            (MqttPacketType)resp_type, publish->packet_id,
                            &publish->pendResp, &publish->resp);
                    wm_SemUnlock(&client->lockClient);
                }
                if (rc != 0) {
                    wm_SemUnlock(&client->lockSend);
                    return rc; /* Error locking client */
                }
            }
        #endif

            publish->stat = MQTT_MSG_WRITE;
        }
        FALL_THROUGH;

        case MQTT_MSG_WRITE:
        {
            /* Send packet and payload */
            rc = MqttPacket_Write(client, client->tx_buf,
                    client->write.len);
        #ifdef WOLFMQTT_NONBLOCK
            if (rc == MQTT_CODE_CONTINUE)
                return rc;
        #endif
        #ifdef WOLFMQTT_MULTITHREAD
            wm_SemUnlock(&client->lockSend);
        #endif

            if (rc < 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                if (wm_SemLock(&client->lockClient) == 0) {
                    MqttClient_RespList_Remove(client, &publish->pendResp);
                    wm_SemUnlock(&client->lockClient);
                }
            #endif
                return rc;
            }

            if (rc == client->write.len) {
                rc = MQTT_CODE_SUCCESS;
            }
            else {
                rc = -1;
            }

            /* if not expecting a reply, the reset state and exit */
            if ((publish->qos == MQTT_QOS_0) ||
                (publish->qos == MQTT_QOS_3)) {
                break;
            }

            publish->stat = MQTT_MSG_WAIT;
        }
        FALL_THROUGH;

        case MQTT_MSG_WAIT:
        {
            /* Handle QoS */
            if ((publish->qos == MQTT_QOS_1) ||
                (publish->qos == MQTT_QOS_2)) {

                /* Determine packet type to wait for */
                resp_type = (publish->qos == MQTT_QOS_1) ?
                        SN_MSG_TYPE_PUBACK :
                        SN_MSG_TYPE_PUBCOMP;

                /* Wait for publish response packet */
                rc = SN_Client_WaitType(client, &publish->resp,
                    resp_type, publish->packet_id, client->cmd_timeout_ms);
            #ifdef WOLFMQTT_NONBLOCK
                if (rc == MQTT_CODE_CONTINUE)
                    break;
            #endif
            #ifdef WOLFMQTT_MULTITHREAD
                if (wm_SemLock(&client->lockClient) == 0) {
                    MqttClient_RespList_Remove(client, &publish->pendResp);
                    wm_SemUnlock(&client->lockClient);
                }
            #endif

                publish->return_code = publish->resp.return_code;
            }

            break;
        }

        case MQTT_MSG_READ:
        case MQTT_MSG_READ_PAYLOAD:
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("SN_Client_Publish: Invalid state %d!",
                publish->stat);
        #endif
            rc = MQTT_CODE_ERROR_STAT;
            break;
    } /* switch (publish->stat) */

    /* reset state */
#ifdef WOLFMQTT_NONBLOCK
    if (rc != MQTT_CODE_CONTINUE)
#endif
    {
        publish->stat = MQTT_MSG_BEGIN;
    }
    if (rc > 0) {
        rc = MQTT_CODE_SUCCESS;
    }

    return rc;
}