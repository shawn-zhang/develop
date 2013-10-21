# This is a generic product that isn't specialized for a specific device.
# It includes the base Android platform. If you need Google-specific features,
# you should derive from generic_with_google.mk

$(call inherit-product, $(SRC_TARGET_DIR)/product/generic.mk)

PRODUCT_PACKAGES += \
	LedTest

PRODUCT_COPY_FILES := \
    vendor/mokoid/frameworks/base/service/com.mokoid.server.xml:system/etc/permissions/com.mokoid.server.xml

# If we get a real board, put these in.
#PRODUCT_COPY_FILES += \
#    vendor/mokoid/dma6410xp/init.rc:root/init.rc \
#    vendor/mokoid/dma6410xp/init.dma6410xp.rc:root/init.dma6410xp.rc

# Overrides (see build.prop for results)
PRODUCT_NAME := dma6410xp
PRODUCT_DEVICE := dma6410xp
