int MqttClient_WaitMessage_ex(MqttClient *client, MqttObject* msg,
        int timeout_ms)
{
    return MqttClient_WaitType(client, msg, MQTT_PACKET_TYPE_ANY, 0,
        timeout_ms);
}