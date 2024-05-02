
#include "webvtt_decoder.h"

namespace webvtt
{

	WebVTTDecoder::WebVTTDecoder()
	{
	}
	WebVTTDecoder::~WebVTTDecoder()
	{
	}
	bool WebVTTDecoder::AddPacket(const std::shared_ptr<const ov::Data>& packet)
	{
		return false;
	}
	bool WebVTTDecoder::IsCueAvailable()
	{
		return false;
	}
	const std::shared_ptr<webvtt::Cue> WebVTTDecoder::GetNextCue()
	{
		return std::shared_ptr<webvtt::Cue>();
	}
}
