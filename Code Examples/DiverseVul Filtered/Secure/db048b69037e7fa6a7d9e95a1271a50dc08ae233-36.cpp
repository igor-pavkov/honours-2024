int dev_ethtool(struct net *net, struct ifreq *ifr)
{
	struct net_device *dev = __dev_get_by_name(net, ifr->ifr_name);
	void __user *useraddr = ifr->ifr_data;
	u32 ethcmd;
	int rc;
	unsigned long old_features;

	if (!dev || !netif_device_present(dev))
		return -ENODEV;

	if (!dev->ethtool_ops)
		return -EOPNOTSUPP;

	if (copy_from_user(&ethcmd, useraddr, sizeof(ethcmd)))
		return -EFAULT;

	/* Allow some commands to be done by anyone */
	switch (ethcmd) {
	case ETHTOOL_GDRVINFO:
	case ETHTOOL_GMSGLVL:
	case ETHTOOL_GCOALESCE:
	case ETHTOOL_GRINGPARAM:
	case ETHTOOL_GPAUSEPARAM:
	case ETHTOOL_GRXCSUM:
	case ETHTOOL_GTXCSUM:
	case ETHTOOL_GSG:
	case ETHTOOL_GSTRINGS:
	case ETHTOOL_GTSO:
	case ETHTOOL_GPERMADDR:
	case ETHTOOL_GUFO:
	case ETHTOOL_GGSO:
	case ETHTOOL_GGRO:
	case ETHTOOL_GFLAGS:
	case ETHTOOL_GPFLAGS:
	case ETHTOOL_GRXFH:
	case ETHTOOL_GRXRINGS:
	case ETHTOOL_GRXCLSRLCNT:
	case ETHTOOL_GRXCLSRULE:
	case ETHTOOL_GRXCLSRLALL:
		break;
	default:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
	}

	if (dev->ethtool_ops->begin) {
		rc = dev->ethtool_ops->begin(dev);
		if (rc  < 0)
			return rc;
	}
	old_features = dev->features;

	switch (ethcmd) {
	case ETHTOOL_GSET:
		rc = ethtool_get_settings(dev, useraddr);
		break;
	case ETHTOOL_SSET:
		rc = ethtool_set_settings(dev, useraddr);
		break;
	case ETHTOOL_GDRVINFO:
		rc = ethtool_get_drvinfo(dev, useraddr);
		break;
	case ETHTOOL_GREGS:
		rc = ethtool_get_regs(dev, useraddr);
		break;
	case ETHTOOL_GWOL:
		rc = ethtool_get_wol(dev, useraddr);
		break;
	case ETHTOOL_SWOL:
		rc = ethtool_set_wol(dev, useraddr);
		break;
	case ETHTOOL_GMSGLVL:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       dev->ethtool_ops->get_msglevel);
		break;
	case ETHTOOL_SMSGLVL:
		rc = ethtool_set_value_void(dev, useraddr,
				       dev->ethtool_ops->set_msglevel);
		break;
	case ETHTOOL_NWAY_RST:
		rc = ethtool_nway_reset(dev);
		break;
	case ETHTOOL_GLINK:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       dev->ethtool_ops->get_link);
		break;
	case ETHTOOL_GEEPROM:
		rc = ethtool_get_eeprom(dev, useraddr);
		break;
	case ETHTOOL_SEEPROM:
		rc = ethtool_set_eeprom(dev, useraddr);
		break;
	case ETHTOOL_GCOALESCE:
		rc = ethtool_get_coalesce(dev, useraddr);
		break;
	case ETHTOOL_SCOALESCE:
		rc = ethtool_set_coalesce(dev, useraddr);
		break;
	case ETHTOOL_GRINGPARAM:
		rc = ethtool_get_ringparam(dev, useraddr);
		break;
	case ETHTOOL_SRINGPARAM:
		rc = ethtool_set_ringparam(dev, useraddr);
		break;
	case ETHTOOL_GPAUSEPARAM:
		rc = ethtool_get_pauseparam(dev, useraddr);
		break;
	case ETHTOOL_SPAUSEPARAM:
		rc = ethtool_set_pauseparam(dev, useraddr);
		break;
	case ETHTOOL_GRXCSUM:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_rx_csum ?
					dev->ethtool_ops->get_rx_csum :
					ethtool_op_get_rx_csum));
		break;
	case ETHTOOL_SRXCSUM:
		rc = ethtool_set_rx_csum(dev, useraddr);
		break;
	case ETHTOOL_GTXCSUM:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_tx_csum ?
					dev->ethtool_ops->get_tx_csum :
					ethtool_op_get_tx_csum));
		break;
	case ETHTOOL_STXCSUM:
		rc = ethtool_set_tx_csum(dev, useraddr);
		break;
	case ETHTOOL_GSG:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_sg ?
					dev->ethtool_ops->get_sg :
					ethtool_op_get_sg));
		break;
	case ETHTOOL_SSG:
		rc = ethtool_set_sg(dev, useraddr);
		break;
	case ETHTOOL_GTSO:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_tso ?
					dev->ethtool_ops->get_tso :
					ethtool_op_get_tso));
		break;
	case ETHTOOL_STSO:
		rc = ethtool_set_tso(dev, useraddr);
		break;
	case ETHTOOL_TEST:
		rc = ethtool_self_test(dev, useraddr);
		break;
	case ETHTOOL_GSTRINGS:
		rc = ethtool_get_strings(dev, useraddr);
		break;
	case ETHTOOL_PHYS_ID:
		rc = ethtool_phys_id(dev, useraddr);
		break;
	case ETHTOOL_GSTATS:
		rc = ethtool_get_stats(dev, useraddr);
		break;
	case ETHTOOL_GPERMADDR:
		rc = ethtool_get_perm_addr(dev, useraddr);
		break;
	case ETHTOOL_GUFO:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_ufo ?
					dev->ethtool_ops->get_ufo :
					ethtool_op_get_ufo));
		break;
	case ETHTOOL_SUFO:
		rc = ethtool_set_ufo(dev, useraddr);
		break;
	case ETHTOOL_GGSO:
		rc = ethtool_get_gso(dev, useraddr);
		break;
	case ETHTOOL_SGSO:
		rc = ethtool_set_gso(dev, useraddr);
		break;
	case ETHTOOL_GFLAGS:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       (dev->ethtool_ops->get_flags ?
					dev->ethtool_ops->get_flags :
					ethtool_op_get_flags));
		break;
	case ETHTOOL_SFLAGS:
		rc = ethtool_set_value(dev, useraddr,
				       dev->ethtool_ops->set_flags);
		break;
	case ETHTOOL_GPFLAGS:
		rc = ethtool_get_value(dev, useraddr, ethcmd,
				       dev->ethtool_ops->get_priv_flags);
		break;
	case ETHTOOL_SPFLAGS:
		rc = ethtool_set_value(dev, useraddr,
				       dev->ethtool_ops->set_priv_flags);
		break;
	case ETHTOOL_GRXFH:
	case ETHTOOL_GRXRINGS:
	case ETHTOOL_GRXCLSRLCNT:
	case ETHTOOL_GRXCLSRULE:
	case ETHTOOL_GRXCLSRLALL:
		rc = ethtool_get_rxnfc(dev, useraddr);
		break;
	case ETHTOOL_SRXFH:
	case ETHTOOL_SRXCLSRLDEL:
	case ETHTOOL_SRXCLSRLINS:
		rc = ethtool_set_rxnfc(dev, useraddr);
		break;
	case ETHTOOL_GGRO:
		rc = ethtool_get_gro(dev, useraddr);
		break;
	case ETHTOOL_SGRO:
		rc = ethtool_set_gro(dev, useraddr);
		break;
	case ETHTOOL_FLASHDEV:
		rc = ethtool_flash_device(dev, useraddr);
		break;
	case ETHTOOL_RESET:
		rc = ethtool_reset(dev, useraddr);
		break;
	case ETHTOOL_SRXNTUPLE:
		rc = ethtool_set_rx_ntuple(dev, useraddr);
		break;
	case ETHTOOL_GRXNTUPLE:
		rc = ethtool_get_rx_ntuple(dev, useraddr);
		break;
	case ETHTOOL_GSSET_INFO:
		rc = ethtool_get_sset_info(dev, useraddr);
		break;
	default:
		rc = -EOPNOTSUPP;
	}

	if (dev->ethtool_ops->complete)
		dev->ethtool_ops->complete(dev);

	if (old_features != dev->features)
		netdev_features_change(dev);

	return rc;
}