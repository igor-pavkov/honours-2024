static inline int MqttIsPubRespPacket(int packet_type)
{
    return (packet_type == MQTT_PACKET_TYPE_PUBLISH_ACK /* Acknowledgment */ ||
            packet_type == MQTT_PACKET_TYPE_PUBLISH_REC /* Received */ ||
            packet_type == MQTT_PACKET_TYPE_PUBLISH_REL /* Release */ ||
            packet_type == MQTT_PACKET_TYPE_PUBLISH_COMP /* Complete */);
}