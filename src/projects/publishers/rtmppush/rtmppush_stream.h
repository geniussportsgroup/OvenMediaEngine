#pragma once

#include <base/common_types.h>
#include <base/publisher/stream.h>
#include "monitoring/monitoring.h"
#include "rtmppush_session.h"

class RtmpPushStream : public pub::Stream
{
public:
	static std::shared_ptr<RtmpPushStream> Create(const std::shared_ptr<pub::Application> application,
												  const info::Stream &info);

	explicit RtmpPushStream(const std::shared_ptr<pub::Application> application,
							const info::Stream &info);
	~RtmpPushStream() final;

	void SendFrame(const std::shared_ptr<MediaPacket> &media_packet);
	void SendVideoFrame(const std::shared_ptr<MediaPacket> &media_packet) override;
	void SendAudioFrame(const std::shared_ptr<MediaPacket> &media_packet) override;
	void SendDataFrame(const std::shared_ptr<MediaPacket> &media_packet) override {}  // Not supported
	void SendSubtitleFrame(const std::shared_ptr<MediaPacket> &media_packet) override {} // Not supported

	std::shared_ptr<pub::Session> CreatePushSession(std::shared_ptr<info::Push> &push) override;

private:
	bool Start() override;
	bool Stop() override;

	std::shared_ptr<mon::StreamMetrics> _stream_metrics;
};
