int MqttClient_Publish_ex(MqttClient *client, MqttPublish *publish,
                            MqttPublishCb pubCb)
{
    int rc = MQTT_CODE_SUCCESS;
    MqttPacketType resp_type;

    /* Validate required arguments */
    if (client == NULL || publish == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

#ifdef WOLFMQTT_V5
    /* Use specified protocol version if set */
    publish->protocol_level = client->protocol_level;

    /* Validate publish request against server properties */
    if ((publish->qos > client->max_qos) ||
        ((publish->retain == 1) && (client->retain_avail == 0)))
    {
        return MQTT_CODE_ERROR_SERVER_PROP;
    }
#endif

    switch (publish->stat)
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
            rc = MqttEncode_Publish(client->tx_buf, client->tx_buf_len,
                    publish, pubCb ? 1 : 0);
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d,"
                    " QoS %d",
                rc, MqttPacket_TypeDesc(MQTT_PACKET_TYPE_PUBLISH),
                MQTT_PACKET_TYPE_PUBLISH, publish->packet_id,
                publish->qos);
        #endif
            if (rc <= 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                wm_SemUnlock(&client->lockSend);
            #endif
                return rc;
            }
            client->write.len = rc;

        #ifdef WOLFMQTT_MULTITHREAD
            if (publish->qos > MQTT_QOS_0) {
                resp_type = (publish->qos == MQTT_QOS_1) ?
                        MQTT_PACKET_TYPE_PUBLISH_ACK :
                        MQTT_PACKET_TYPE_PUBLISH_COMP;

                rc = wm_SemLock(&client->lockClient);
                if (rc == 0) {
                    /* inform other threads of expected response */
                    rc = MqttClient_RespList_Add(client, resp_type,
                        publish->packet_id, &publish->pendResp, &publish->resp);
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
            /* Send packet */
            rc = MqttPacket_Write(client, client->tx_buf, client->write.len);
        #ifdef WOLFMQTT_NONBLOCK
            if (rc == MQTT_CODE_CONTINUE)
                return rc;
        #endif
            if (rc < 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                wm_SemUnlock(&client->lockSend);
            #endif
            #ifdef WOLFMQTT_MULTITHREAD
                if (wm_SemLock(&client->lockClient) == 0) {
                    MqttClient_RespList_Remove(client, &publish->pendResp);
                    wm_SemUnlock(&client->lockClient);
                }
            #endif
                return rc;
            }

            /* advance state */
            publish->stat = MQTT_MSG_WRITE_PAYLOAD;
        }
        FALL_THROUGH;

        case MQTT_MSG_WRITE_PAYLOAD:
        {
            rc = MqttClient_Publish_WritePayload(client, publish, pubCb);
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
                break;
            }

            /* if not expecting a reply then we are done */
            if (publish->qos == MQTT_QOS_0) {
                break;
            }
            publish->stat = MQTT_MSG_WAIT;
        }
        FALL_THROUGH;

        case MQTT_MSG_WAIT:
        {
            /* Handle QoS */
            if (publish->qos > MQTT_QOS_0) {
                /* Determine packet type to wait for */
                resp_type = (publish->qos == MQTT_QOS_1) ?
                    MQTT_PACKET_TYPE_PUBLISH_ACK :
                    MQTT_PACKET_TYPE_PUBLISH_COMP;

                /* Wait for publish response packet */
                rc = MqttClient_WaitType(client, &publish->resp, resp_type,
                    publish->packet_id, client->cmd_timeout_ms);
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
            }
            break;
        }

    #ifdef WOLFMQTT_V5
        case MQTT_MSG_AUTH:
    #endif
        case MQTT_MSG_READ:
        case MQTT_MSG_READ_PAYLOAD:
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("MqttClient_Publish: Invalid state %d!",
                publish->stat);
        #endif
            rc = MQTT_CODE_ERROR_STAT;
            break;
    } /* switch (publish->stat) */

    /* reset state */
    if ((rc != MQTT_CODE_PUB_CONTINUE)
#ifdef WOLFMQTT_NONBLOCK
         && (rc != MQTT_CODE_CONTINUE)
#endif
        )
    {
        publish->stat = MQTT_MSG_BEGIN;
    }
    if (rc > 0) {
        rc = MQTT_CODE_SUCCESS;
    }

    return rc;
}