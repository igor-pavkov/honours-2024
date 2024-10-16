int MqttClient_Publish(MqttClient *client, MqttPublish *publish)
{
    return MqttClient_Publish_ex(client, publish, NULL);
}