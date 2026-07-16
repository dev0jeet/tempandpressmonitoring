# SPDX-FileCopyrightText: Copyright (C) Arduino s.r.l. and/or its affiliated companies
#
# SPDX-License-Identifier: MPL-2.0

import time
import os
from datetime import datetime
from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI

sensor_data = {"temperature": 0.0, "pressure": 0.0}
sampling_interval = 2  # default loop timing window in seconds



def poll_sensor():
    global sensor_data
    temp_raw = Bridge.call("get_temperature")
    press_raw = Bridge.call("get_pressure")

    if temp_raw is not None and press_raw is not None:
        sensor_data["temperature"] = temp_raw / 100.0
        sensor_data["pressure"] = press_raw / 100.0
        ui.send_message("sensor_data_update", sensor_data)

def change_interval(client, data):
    global sampling_interval
    interval = int(data.get("interval", 2))
    sampling_interval = max(1, interval)
    Bridge.call("set_interval", sampling_interval)

ui = WebUI()
ui.on_message("get_initial_state", lambda c, d: ui.send_message("sensor_data_update", sensor_data, c))
ui.on_message("update_sampling_interval", change_interval)


def loop():
    poll_sensor()
    time.sleep(sampling_interval)

App.run(user_loop=loop)