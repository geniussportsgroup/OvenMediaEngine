//==============================================================================
//
//  WebVTT Stream
//
//  Created by Hyunjun Jang
//  Moved by Getroot
//
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//==============================================================================

#include "base/common_types.h"
#include "base/provider/push_provider/stream.h"
#include "modules/containers/mpegts/mpegts_depacketizer.h"

namespace pvd
{
	class WebVTTStream : public pvd::PushStream
	{
	public:
		static std::shared_ptr<WebVTTStream> Create(StreamSourceType source_type, uint32_t channel_id, const std::shared_ptr<ov::Socket> &client_socket, const std::shared_ptr<PushProvider> &provider);

		explicit WebVTTStream(StreamSourceType source_type, uint32_t channel_id, std::shared_ptr<ov::Socket> client_socket, const std::shared_ptr<PushProvider> &provider);
		~WebVTTStream() final;

		bool Stop() override;

		const std::shared_ptr<ov::Socket>&	GetClientSock();

		// ------------------------------------------
		// Implementation of PushStream
		// ------------------------------------------
		PushStreamType GetPushStreamType() override
		{
			return PushStream::PushStreamType::INTERLEAVED;
		}
		bool OnDataReceived(const std::shared_ptr<const ov::Data> &data) override;

	private:
		bool Start() override;

		// Client socket
		std::shared_ptr<ov::Socket> _remote = nullptr;

		info::VHostAppName _vhost_app_name;
	};
}