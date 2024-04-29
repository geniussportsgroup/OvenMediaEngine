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
		if(GetState() == Stream::State::ERROR || GetState() == Stream::State::STOPPED)
		{
			return false;
		}

		if (_lifetime_epoch_msec != 0 &&
			_remote->GetType() == ov::SocketType::Srt &&
			_lifetime_epoch_msec < ov::Clock::NowMSec())
		{
			// Expired
			logti("Stream has expired by signed policy (%s/%s)", _vhost_app_name.CStr(), GetName().CStr());
			Stop();
			return false;
		}

		std::lock_guard<std::shared_mutex> lock(_depacketizer_lock);
		_depacketizer.AddPacket(data);

		// Publish
		if (IsPublished() == false && _depacketizer.IsTrackInfoAvailable())
		{
			if (Publish() == false)
			{
				return false;
			}
		}

		if (IsPublished() == true)
		{
			while (_depacketizer.IsESAvailable())
			{
				auto es = _depacketizer.PopES();
				auto track = GetTrack(es->PID());

				if (track == nullptr)
				{
					logte("%s/%s(%d) received stream data, but track information could not be found.", GetApplicationName(), GetName().CStr(), GetId());
					return false;
				}

				int64_t origin_pts = es->Pts();
				int64_t origin_dts = es->Dts();
				auto pts = origin_pts;
				auto dts = origin_dts;

				AdjustTimestampByBase(track->GetId(), pts, dts, 0x1FFFFFFFFLL);

				if (es->IsVideoStream())
				{
					auto bitstream = cmn::BitstreamFormat::Unknown;
					auto packet_type = cmn::PacketType::NALU;

					switch (track->GetCodecId())
					{
						case cmn::MediaCodecId::H264:
							bitstream = cmn::BitstreamFormat::H264_ANNEXB;
							break;
						case cmn::MediaCodecId::H265: {
							bitstream = cmn::BitstreamFormat::H265_ANNEXB;
							break;
						}
						default:
							bitstream = cmn::BitstreamFormat::Unknown;
							break;
					}

					auto data = std::make_shared<ov::Data>(es->Payload(), es->PayloadLength());
					auto media_packet = std::make_shared<MediaPacket>(GetMsid(),
																	  cmn::MediaType::Video,
																	  es->PID(),
																	  data,
																	  pts,
																	  dts,
																	  bitstream,
																	  packet_type);
					SendFrame(media_packet);
				}
				else if (es->IsAudioStream())
				{
					auto payload = es->Payload();
					auto payload_length = es->PayloadLength();
				
					auto data = std::make_shared<ov::Data>(payload, payload_length);
					auto media_packet = std::make_shared<MediaPacket>(GetMsid(),
																	  cmn::MediaType::Audio,
																	  es->PID(),
																	  data,
																	  pts,
																	  dts,
																	  cmn::BitstreamFormat::AAC_ADTS,
																	  cmn::PacketType::RAW);
					SendFrame(media_packet);
				}

				logtd("Frame - PID(%d) AdjustPTS(%lld) AdjustDTS(%lld) PTS(%lld) DTS(%lld) Size(%d)", es->PID(), pts, dts, origin_pts, origin_dts, es->PayloadLength());
			}
		}

		return true;
	}

	bool WebVTTStream::Publish()
	{
		std::map<uint16_t, std::shared_ptr<MediaTrack>> track_list;

		if (_depacketizer.GetTrackList(&track_list) == false)
		{
			logte("Cannot get track list from mpeg-ts depacketizer.");
			return false;
		}

		for (const auto &x : track_list)
		{
			auto track = x.second;
			AddTrack(track);
		}

		// Publish
		if (PublishChannel(_vhost_app_name) == false)
		{
			return false;
		}

		return true;
	}
}  // namespace pvd