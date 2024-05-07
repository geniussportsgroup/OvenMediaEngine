LOCAL_PATH := $(call get_local_path)
include $(DEFAULT_VARIABLES)

LOCAL_STATIC_LIBRARIES := \
	webvtt_subtitle

LOCAL_TARGET := webvtt_provider

$(call add_pkg_config,srt)

include $(BUILD_STATIC_LIBRARY)