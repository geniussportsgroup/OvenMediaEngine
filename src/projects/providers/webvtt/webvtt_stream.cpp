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

// Fix track id
#define WEBVTT_TRACK_ID		0

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
		_name = "WebVTTInput";
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

		if (IsPublished() == false)
		{
			Publish();
		}

		for(std::shared_ptr<webvtt::Cue> cue: *_decoder->GetNextCues()) {
			auto cueData = std::make_shared<ov::Data>(cue->_text.length());
			cueData->Append(cue->_text.c_str(), cue->_text.length());

			auto media_packet = std::make_shared<MediaPacket>(GetMsid(),
															  cmn::MediaType::Subtitle,
															  WEBVTT_TRACK_ID,
															  cueData,
															  cue->_time_start,
															  cue->_duration,
															  cmn::BitstreamFormat::WebVTT,
															  cmn::PacketType::SUBTITLE_EVENT);

			return SendFrame(media_packet);
		}

		return true;
	}

	bool WebVTTStream::Publish()
	{
		auto subtitle_track = std::make_shared<MediaTrack>();

		subtitle_track->SetId(WEBVTT_TRACK_ID);
		subtitle_track->SetMediaType(cmn::MediaType::Subtitle);
		subtitle_track->SetTimeBase(1, 1000);
		subtitle_track->SetOriginBitstream(cmn::BitstreamFormat::WebVTT);

		AddTrack(subtitle_track);

		//TODO we don't know how to get this info
		_vhost_app_name = ocst::Orchestrator::GetInstance()->ResolveApplicationNameFromDomain("app", "app");

		PublishChannel(_vhost_app_name);

		return true;
	}
}  // namespace pvd