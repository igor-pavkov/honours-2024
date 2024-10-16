int MqttClient_GetProtocolVersion(MqttClient *client)
{
#ifdef WOLFMQTT_V5
    if (client && client->protocol_level == MQTT_CONNECT_PROTOCOL_LEVEL_5)
        return MQTT_CONNECT_PROTOCOL_LEVEL_5;
#else
    (void)client;
#endif
    return MQTT_CONNECT_PROTOCOL_LEVEL_4;
}