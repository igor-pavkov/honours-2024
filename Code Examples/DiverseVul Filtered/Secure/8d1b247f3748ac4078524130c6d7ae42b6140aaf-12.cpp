uint64_t vhost_vsock_common_get_features(VirtIODevice *vdev, uint64_t features,
                                         Error **errp)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);

    if (vvc->seqpacket != ON_OFF_AUTO_OFF) {
        virtio_add_feature(&features, VIRTIO_VSOCK_F_SEQPACKET);
    }

    features = vhost_get_features(&vvc->vhost_dev, feature_bits, features);

    if (vvc->seqpacket == ON_OFF_AUTO_ON &&
        !virtio_has_feature(features, VIRTIO_VSOCK_F_SEQPACKET)) {
        error_setg(errp, "vhost-vsock backend doesn't support seqpacket");
    }

    return features;
}