#!/bin/bash

source system/tools/hidl/update-makefiles-helper.sh

do_makefiles_update \
  "vendor.silead.hardware:vendor/silead/hardware/fingerprintHidl" \
  "android.hidl:system/libhidl/transport"

