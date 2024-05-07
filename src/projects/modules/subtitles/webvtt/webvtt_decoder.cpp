
#include "webvtt_decoder.h"
#include "webvtt_cue.h"

namespace webvtt
{

	WebVTTDecoder::WebVTTDecoder()
	{
		_currentData = std::make_shared<ov::Data>();
		_nextCues = std::make_shared<std::vector<std::shared_ptr<Cue>>>();
	}

	WebVTTDecoder::~WebVTTDecoder()
	{
	}

	bool WebVTTDecoder::AddPacket(const std::shared_ptr<const ov::Data> &packet)
	{
		_currentData->Append(packet);

		auto lines = GetLines();

		if (!_headRead)
		{
			auto headerLine = lines->front();

			if (headerLine == _header)
			{
				_headRead = true;
			}
			else
			{
				return true;
			}

			lines->pop_front();
			_currentData->Erase(0, headerLine.length() + 2);
		}

		while(!lines->empty()){
			auto timeInterval = lines->front();
			lines->pop_front();

			if(lines->empty()) {
				return true;
			}

			auto subtitlesText = lines->front();
			lines->pop_front();

			auto cue = std::make_shared<webvtt::Cue>();

			if(!cue->Parse(timeInterval, subtitlesText)){
				return true;
			}

			_nextCues->push_back(cue);

			_currentData->Erase(0, timeInterval.length() + 1 + subtitlesText.length() + 1 + 1);
		}

		return true;
	}

	bool WebVTTDecoder::IsCueAvailable()
	{
		return !_nextCues->empty();
	}

	std::shared_ptr<std::vector<std::shared_ptr<webvtt::Cue>>> WebVTTDecoder::GetNextCues()
	{
		return _nextCues;
	}

	bool WebVTTDecoder::ClearCurrentCues()
	{
		_nextCues->clear();

		return true;
	}

	std::shared_ptr<std::deque<std::string>> WebVTTDecoder::GetLines()
	{
		auto lines = std::make_shared<std::deque<std::string>>();

		auto charText = _currentData->GetDataAs<char>();

		auto text = std::string(charText);

		auto startLineIndex = 0;
		auto foundLineDelimiterIndex = text.find(_lineDelimiter, startLineIndex);

		while (foundLineDelimiterIndex != std::string::npos)
		{
			auto numberOfCharacters = foundLineDelimiterIndex - startLineIndex;
			auto line = text.substr(startLineIndex, numberOfCharacters);

			if (!line.empty() && line != _lineDelimiter)
			{
				lines->push_back(line);
			}

			startLineIndex = foundLineDelimiterIndex + 1;
			foundLineDelimiterIndex = text.find(_lineDelimiter, startLineIndex);
		}

		return lines;
	}
}  // namespace webvtt
