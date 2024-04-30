//==============================================================================
//
//  WebVTT Stream
//
//  Created by Hyunjun Jang
//  Moved by Getroot
//
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//==============================================================================

#include "webvtt_stream.h"

#include <base/info/media_extradata.h>
#include <base/mediarouter/media_type.h>
#include <orchestrator/orchestrator.h>

#include "base/info/application.h"
#include "base/provider/push_provider/application.h"
#include "modules/bitstream/aac/aac_adts.h"
#include "modules/bitstream/h265/h265_parser.h"
#include "modules/bitstream/nalu/nal_unit_splitter.h"
#include "modules/containers/mpegts/mpegts_packet.h"
#include "webvtt_provider_private.h"

namespace pvd
{
	std::shared_ptr<WebVTTStream> WebVTTStream::Create(StreamSourceType source_type, uint32_t channel_id, const std::shared_ptr<ov::Socket> &client_socket, const std::shared_ptr<PushProvider> &provider)
	{
		auto stream = std::make_shared<WebVTTStream>(source_type, channel_id, client_socket, provider);
		if (stream != nullptr)
		{
			stream->Start();
		}
		return stream;
	}

	WebVTTStream::WebVTTStream(StreamSourceType source_type, uint32_t channel_id, std::shared_ptr<ov::Socket> client_socket, const std::shared_ptr<PushProvider> &provider)
		: PushStream(source_type, channel_id, provider),

		  _vhost_app_name(info::VHostAppName::InvalidVHostAppName())
	{

	}

	WebVTTStream::~WebVTTStream()
	{
	}

	bool WebVTTStream::Start()
	{
		SetState(Stream::State::PLAYING);
		return PushStream::Start();
	}

	bool WebVTTStream::Stop()
	{
		if (GetState() == Stream::State::STOPPED)
		{
			return true;
		}

		if(_remote->GetState() == ov::SocketState::Connected)
		{
			_remote->Close();
		}

		return PushStream::Stop();
	}

	const std::shared_ptr<ov::Socket> &WebVTTStream::GetClientSock()
	{
		return _remote;
	}

	bool WebVTTStream::OnDataReceived(const std::shared_ptr<const ov::Data> &data)
	{
		return true;
	}
}  // namespace pvd