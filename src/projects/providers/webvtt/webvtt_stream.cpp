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
#include "modules/subtitles/webvtt/webvtt_decoder.h"
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
		_decoder = std::make_shared<webvtt::WebVTTDecoder>();
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

	bool WebVTTStream::OnDataReceived(const std::shared_ptr<const ov::Data> &data)
	{
		_decoder->AddPacket(data);

		if (!_decoder->IsCueAvailable()) {
			return true;
		}

		//TODO crear el Data por cada cue y enviar frames

		auto cueData = std::make_shared<ov::Data>();

		auto media_packet = std::make_shared<MediaPacket>(GetMsid(),
														  cmn::MediaType::Subtitle,
														  1,
														  cueData,
														  1,
														  1,
														  cmn::BitstreamFormat::WebVTT,
														  cmn::PacketType::SUBTITLE_EVENT);

		return SendFrame(media_packet);
	}
}  // namespace pvd