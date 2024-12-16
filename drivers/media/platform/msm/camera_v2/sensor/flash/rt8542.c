/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/export.h>
#include "msm_led_flash.h"

#define FLASH_NAME "qcom,led-flash"

/*#define CONFIG_MSMB_CAMERA_DEBUG*/
#undef CDBG
#ifdef CONFIG_MSMB_CAMERA_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

static struct msm_led_flash_ctrl_t fctrl;
static struct i2c_driver rt8542_i2c_driver;

static void __exit msm_flash_rt8542_i2c_remove(void)
{
	i2c_del_driver(&rt8542_i2c_driver);
	return;
}

static const struct of_device_id rt8542_i2c_trigger_dt_match[] = {
	{.compatible = "qcom,led-flash"},
	{}
};

MODULE_DEVICE_TABLE(of, rt8542_i2c_trigger_dt_match);

static const struct i2c_device_id flash_i2c_id[] = {
	{"qcom,led-flash", (kernel_ulong_t)&fctrl},
	{ }
};

static const struct i2c_device_id rt8542_i2c_id[] = {
	{FLASH_NAME, (kernel_ulong_t)&fctrl},
	{ }
};

extern void rt8542_led_enable(void);
extern void rt8542_led_disable(void);

int rt8542_flash_led_release(struct msm_led_flash_ctrl_t *fctrl) {
	msm_flash_led_release(fctrl);
	rt8542_led_disable();
	return 0;
}

int rt8542_flash_led_off(struct msm_led_flash_ctrl_t *fctrl) {
	msm_flash_led_off(fctrl);
	rt8542_led_disable();
	return 0;
}

int rt8542_flash_led_low(struct msm_led_flash_ctrl_t *fctrl) {
	msm_flash_led_low(fctrl);
	rt8542_led_enable();
	return 0;
}

int rt8542_flash_led_high(struct msm_led_flash_ctrl_t *fctrl) {
	msm_flash_led_high(fctrl);
	rt8542_led_enable();
	return 0;
}

static int msm_flash_rt8542_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	if (!id) {
		pr_err("msm_flash_rt8542_i2c_probe: id is NULL");
		id = rt8542_i2c_id;
	}

	return msm_flash_i2c_probe(client, id);
}

static struct i2c_driver rt8542_i2c_driver = {
	.id_table = rt8542_i2c_id,
	.probe  = msm_flash_rt8542_i2c_probe,
	.remove = __exit_p(msm_flash_rt8542_i2c_remove),
	.driver = {
		.name = FLASH_NAME,
		.owner = THIS_MODULE,
		.of_match_table = rt8542_i2c_trigger_dt_match,
	},
};

static int __init msm_flash_rt8542_i2c_add_driver(void)
{
	CDBG("%s called\n", __func__);
	return i2c_add_driver(&rt8542_i2c_driver);
}

static struct msm_camera_i2c_client rt8542_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
};

static struct msm_flash_fn_t rt8542_func_tbl = {
	.flash_led_release = msm_flash_led_release,
	.flash_led_off = msm_flash_led_off,
	.flash_led_low = msm_flash_led_low,
	.flash_led_high = msm_flash_led_high,
};

static struct msm_led_flash_ctrl_t fctrl = {
	.flash_i2c_client = &rt8542_i2c_client,
	.func_tbl = &rt8542_func_tbl,
};

/*subsys_initcall(msm_flash_i2c_add_driver);*/
module_init(msm_flash_rt8542_i2c_add_driver);
module_exit(msm_flash_rt8542_i2c_remove);
MODULE_DESCRIPTION("rt8542 FLASH");
MODULE_LICENSE("GPL v2");
