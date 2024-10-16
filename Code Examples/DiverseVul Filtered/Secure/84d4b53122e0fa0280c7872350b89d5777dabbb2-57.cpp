static int MqttClient_HandlePacket(MqttClient* client,
    MqttPacketType packet_type, void *packet_obj, int timeout_ms)
{
    int rc = MQTT_CODE_SUCCESS;
    MqttQoS packet_qos = MQTT_QOS_0;
    word16 packet_id = 0;

    if (client == NULL || packet_obj == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    switch (packet_type)
    {
        case MQTT_PACKET_TYPE_CONNECT_ACK:
        {
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
            break;
        }
        case MQTT_PACKET_TYPE_PUBLISH:
        {
            MqttPublish *publish = (MqttPublish*)packet_obj;
            MqttPacketType resp_type;

            if (publish->stat == MQTT_MSG_BEGIN ||
                publish->stat == MQTT_MSG_READ) {
                rc = MqttClient_DecodePacket(client, client->rx_buf,
                    client->packet.buf_len, packet_obj, &packet_type,
                    &packet_qos, &packet_id);
                if (rc <= 0) {
                    return rc;
                }
            }
            else {
                /* packet ID and QoS were already established */
                packet_id = client->msg.publish.packet_id;
                packet_qos = client->msg.publish.qos;
            }

            rc = MqttClient_Publish_ReadPayload(client, publish, timeout_ms);
            if (rc < 0) {
                break;
            }

            /* Handle QoS */
            if (packet_qos == MQTT_QOS_0) {
                /* we are done, no QoS response */
                break;
            }

            /* Determine packet type to write */
            resp_type = (packet_qos == MQTT_QOS_1) ?
                MQTT_PACKET_TYPE_PUBLISH_ACK :
                MQTT_PACKET_TYPE_PUBLISH_REC;
            publish->resp.packet_id = packet_id;

        #ifdef WOLFMQTT_MULTITHREAD
            /* Lock send socket mutex */
            rc = wm_SemLock(&client->lockSend);
            if (rc != 0) {
                return rc;
            }
        #endif

            /* Encode publish response */
            rc = MqttEncode_PublishResp(client->tx_buf, client->tx_buf_len,
                resp_type, &publish->resp);
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d,"
                    " QoS %d",
                rc, MqttPacket_TypeDesc(resp_type), resp_type, packet_id,
                packet_qos);
        #endif
            if (rc <= 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                wm_SemUnlock(&client->lockSend);
            #endif
                return rc;
            }
            client->packet.buf_len = rc;

            /* Send publish response packet */
            rc = MqttPacket_Write(client, client->tx_buf,
                client->packet.buf_len);

        #ifdef WOLFMQTT_MULTITHREAD
            wm_SemUnlock(&client->lockSend);
        #endif
            break;
        }
        case MQTT_PACKET_TYPE_PUBLISH_ACK:
        case MQTT_PACKET_TYPE_PUBLISH_REC:
        case MQTT_PACKET_TYPE_PUBLISH_REL:
        case MQTT_PACKET_TYPE_PUBLISH_COMP:
        {
            MqttPublishResp pubRespObj, *publish_resp = &pubRespObj;
            XMEMSET(publish_resp, 0, sizeof(MqttPublishResp));

            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, publish_resp, &packet_type,
                &packet_qos, &packet_id);
            if (rc <= 0) {
                return rc;
            }

            /* If publish Received or Release QoS then proceed */
            if (packet_type != MQTT_PACKET_TYPE_PUBLISH_REC &&
                packet_type != MQTT_PACKET_TYPE_PUBLISH_REL) {
                break;
            }

        #ifdef WOLFMQTT_MULTITHREAD
            /* Lock send socket mutex */
            rc = wm_SemLock(&client->lockSend);
            if (rc != 0) {
                return rc;
            }
        #endif

            /* Encode publish response */
            publish_resp->packet_id = packet_id;
            packet_type = (MqttPacketType)((int)packet_type+1); /* next ack */
        #ifdef WOLFMQTT_V5
            #ifdef WOLFMQTT_DEBUG_CLIENT
                PRINTF("\tPublish response: reason code %d, Type %s (%d),"
                        " ID %d, QoS %d",
                        publish_resp->reason_code,
                        MqttPacket_TypeDesc(packet_type),
                        packet_type, packet_id, packet_qos);
            #endif

            /* return reason code to caller */
            if (packet_obj != NULL) {
                MqttPublishResp* caller_rsp = (MqttPublishResp*)packet_obj;
                caller_rsp->reason_code = publish_resp->reason_code;
            }

            /* Publish QoS response needs success reason code,
             * otherwise will cause disconnect at broker */
            publish_resp->reason_code = MQTT_REASON_SUCCESS;
        #endif

            rc = MqttEncode_PublishResp(client->tx_buf, client->tx_buf_len,
                packet_type, publish_resp);
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("MqttClient_EncodePacket: Len %d, Type %s (%d), ID %d,"
                    " QoS %d",
                rc, MqttPacket_TypeDesc(packet_type), packet_type, packet_id,
                packet_qos);
        #endif
            if (rc <= 0) {
            #ifdef WOLFMQTT_MULTITHREAD
                wm_SemUnlock(&client->lockSend);
            #endif
                return rc;
            }
            client->packet.buf_len = rc;

            /* Send publish response packet */
            rc = MqttPacket_Write(client, client->tx_buf,
                client->packet.buf_len);

        #ifdef WOLFMQTT_MULTITHREAD
            wm_SemUnlock(&client->lockSend);
        #endif
            break;
        }
        case MQTT_PACKET_TYPE_SUBSCRIBE_ACK:
        {
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
            break;
        }
        case MQTT_PACKET_TYPE_UNSUBSCRIBE_ACK:
        {
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
            break;
        }
        case MQTT_PACKET_TYPE_PING_RESP:
        {
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
            break;
        }
        case MQTT_PACKET_TYPE_AUTH:
        {
        #ifdef WOLFMQTT_V5
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
        #else
            rc = MQTT_CODE_ERROR_PACKET_TYPE;
        #endif
            break;
        }

        case MQTT_PACKET_TYPE_DISCONNECT:
        {
        #ifdef WOLFMQTT_V5
            rc = MqttClient_DecodePacket(client, client->rx_buf,
                client->packet.buf_len, packet_obj, &packet_type, &packet_qos,
                &packet_id);
        #else
            rc = MQTT_CODE_ERROR_PACKET_TYPE;
        #endif
            break;
        }
        case MQTT_PACKET_TYPE_CONNECT:
        case MQTT_PACKET_TYPE_SUBSCRIBE:
        case MQTT_PACKET_TYPE_UNSUBSCRIBE:
        case MQTT_PACKET_TYPE_PING_REQ:
        case MQTT_PACKET_TYPE_ANY:
        case MQTT_PACKET_TYPE_RESERVED:
        default:
            /* these types are only sent from client and should not be sent
             * by broker */
            rc = MQTT_CODE_ERROR_PACKET_TYPE;
            break;
    } /* switch (packet_type) */

#ifdef WOLFMQTT_DEBUG_CLIENT
    if (rc < 0) {
        PRINTF("MqttClient_HandlePacket: Rc %d, Type %s (%d), QoS %d, ID %d",
            rc, MqttPacket_TypeDesc(packet_type), packet_type, packet_qos,
            packet_id);
    }
#endif

    return rc;
}