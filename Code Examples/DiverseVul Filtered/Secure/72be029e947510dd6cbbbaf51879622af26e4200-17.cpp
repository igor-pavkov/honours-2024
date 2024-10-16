int hns_roce_init(struct hns_roce_dev *hr_dev)
{
	int ret;
	struct device *dev = hr_dev->dev;

	if (hr_dev->hw->reset) {
		ret = hr_dev->hw->reset(hr_dev, true);
		if (ret) {
			dev_err(dev, "Reset RoCE engine failed!\n");
			return ret;
		}
	}

	if (hr_dev->hw->cmq_init) {
		ret = hr_dev->hw->cmq_init(hr_dev);
		if (ret) {
			dev_err(dev, "Init RoCE Command Queue failed!\n");
			goto error_failed_cmq_init;
		}
	}

	ret = hr_dev->hw->hw_profile(hr_dev);
	if (ret) {
		dev_err(dev, "Get RoCE engine profile failed!\n");
		goto error_failed_cmd_init;
	}

	ret = hns_roce_cmd_init(hr_dev);
	if (ret) {
		dev_err(dev, "cmd init failed!\n");
		goto error_failed_cmd_init;
	}

	ret = hr_dev->hw->init_eq(hr_dev);
	if (ret) {
		dev_err(dev, "eq init failed!\n");
		goto error_failed_eq_table;
	}

	if (hr_dev->cmd_mod) {
		ret = hns_roce_cmd_use_events(hr_dev);
		if (ret) {
			dev_err(dev, "Switch to event-driven cmd failed!\n");
			goto error_failed_use_event;
		}
	}

	ret = hns_roce_init_hem(hr_dev);
	if (ret) {
		dev_err(dev, "init HEM(Hardware Entry Memory) failed!\n");
		goto error_failed_init_hem;
	}

	ret = hns_roce_setup_hca(hr_dev);
	if (ret) {
		dev_err(dev, "setup hca failed!\n");
		goto error_failed_setup_hca;
	}

	if (hr_dev->hw->hw_init) {
		ret = hr_dev->hw->hw_init(hr_dev);
		if (ret) {
			dev_err(dev, "hw_init failed!\n");
			goto error_failed_engine_init;
		}
	}

	ret = hns_roce_register_device(hr_dev);
	if (ret)
		goto error_failed_register_device;

	return 0;

error_failed_register_device:
	if (hr_dev->hw->hw_exit)
		hr_dev->hw->hw_exit(hr_dev);

error_failed_engine_init:
	hns_roce_cleanup_bitmap(hr_dev);

error_failed_setup_hca:
	hns_roce_cleanup_hem(hr_dev);

error_failed_init_hem:
	if (hr_dev->cmd_mod)
		hns_roce_cmd_use_polling(hr_dev);

error_failed_use_event:
	hr_dev->hw->cleanup_eq(hr_dev);

error_failed_eq_table:
	hns_roce_cmd_cleanup(hr_dev);

error_failed_cmd_init:
	if (hr_dev->hw->cmq_exit)
		hr_dev->hw->cmq_exit(hr_dev);

error_failed_cmq_init:
	if (hr_dev->hw->reset) {
		ret = hr_dev->hw->reset(hr_dev, false);
		if (ret)
			dev_err(dev, "Dereset RoCE engine failed!\n");
	}

	return ret;
}