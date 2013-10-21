/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/hardware.h>

#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

/*****************************************************************************/

struct led_module_t {
   struct hw_module_t common;
};

struct led_control_device_t {
   struct hw_device_t common;
   /* supporting control APIs go here */
   int (*set_on)(struct led_control_device_t *dev, int32_t led);
   int (*set_off)(struct led_control_device_t *dev, int32_t led);
};

/*****************************************************************************/

struct led_control_context_t {
	struct led_control_device_t device;
};

#define LED_HARDWARE_MODULE_ID "led"

