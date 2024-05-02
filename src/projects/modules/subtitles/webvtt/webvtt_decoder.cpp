
#include "webvtt_decoder.h"

namespace webvtt
{

	WebVTTDecoder::WebVTTDecoder()
	{
		_currentData = std::make_shared<ov::Data>();
	}

	WebVTTDecoder::~WebVTTDecoder()
	{
	}

	bool WebVTTDecoder::AddPacket(const std::shared_ptr<const ov::Data> &packet)
	{
		_currentData->Append(packet);

		auto charText = _currentData->GetDataAs<char>();

		auto text = std::string(charText);

		if (_headRead == false){
			auto foundLineIndex = text.find(_lineDelimiter);

			if(foundLineIndex == std::string::npos){
				return true;
			}

			auto line = text.substr(0, foundLineIndex);

			if(line == _header){
				_headRead = true;
			} else {
				return false;
			}

			text = text.substr(foundLineIndex + 1);
			_currentData->Erase(0, foundLineIndex + 1);
		}

		do{
			auto foundLineIndex = text.find(_lineDelimiter);

			if(foundLineIndex == std::string::npos){
				return true;
			}

			auto foundTimeIntervalIndex = text.find(_timeIntervalDelimiter);

			if(foundTimeIntervalIndex == std::string::npos){
				return true;
			}

			auto startInterval = text.substr(0, foundTimeIntervalIndex);
			auto endInterval = text.substr(foundTimeIntervalIndex + _timeIntervalDelimiter.length());
			text = text.substr(foundTimeIntervalIndex + 1);

			foundLineIndex = text.find(_lineDelimiter);

			if(foundLineIndex == std::string::npos){
				return true;
			}

			auto subtitle = text.substr(0, foundLineIndex);

			auto cue = std::make_shared<webvtt::Cue>();
			cue->_time_start = startInterval;
			cue->_time_end = endInterval;
			cue->_text = subtitle;
			_nextCues->push_back(cue);

			text = text.substr(foundLineIndex + 1);
			_currentData->Erase(0, foundLineIndex + 1);
	    }
		while(!text.empty());


		return true;
	}

	bool WebVTTDecoder::IsCueAvailable()
	{
		return false;
	}

	const std::shared_ptr<std::vector<std::shared_ptr<webvtt::Cue>>> WebVTTDecoder::GetNextCues()
	{
		return _nextCues;
	}
}  // namespace webvtt
