static int MqttClient_RespList_Find(MqttClient *client,
    MqttPacketType packet_type, word16 packet_id, MqttPendResp **retResp)
{
    int rc = 0;
    MqttPendResp *tmpResp;

    if (client == NULL)
        return MQTT_CODE_ERROR_BAD_ARG;

#ifdef WOLFMQTT_DEBUG_CLIENT
    PRINTF("PendResp Find: Type %s (%d), ID %d",
        MqttPacket_TypeDesc(packet_type), packet_type, packet_id);
#endif

    if (retResp)
        *retResp = NULL; /* clear */

    /* Find pending response entry */
    for (tmpResp = client->firstPendResp;
         tmpResp != NULL;
         tmpResp = tmpResp->next)
    {
        if (packet_type == tmpResp->packet_type &&
           (packet_id == tmpResp->packet_id))
        {
        #ifdef WOLFMQTT_DEBUG_CLIENT
            PRINTF("PendResp Found: %p, Type %s (%d), ID %d",
                tmpResp, MqttPacket_TypeDesc(tmpResp->packet_type),
                tmpResp->packet_type, tmpResp->packet_id);
        #endif

            if (retResp)
                *retResp = tmpResp;
            rc = 1;
            break;
        }
    }
    return rc;
}