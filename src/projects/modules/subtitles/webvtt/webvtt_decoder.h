//==============================================================================
//
//  WebVTT decoder
//
//  Created by Getroot
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <base/ovlibrary/ovlibrary.h>

#include "webvtt_cue.h"

namespace webvtt
{
	class WebVTTDecoder
	{
	public:
		WebVTTDecoder();
		~WebVTTDecoder();

		bool AddPacket(const std::shared_ptr<const ov::Data> &packet);

		bool IsCueAvailable();

		std::shared_ptr<std::vector<std::shared_ptr<webvtt::Cue>>> GetNextCues();
		bool ClearCurrentCues();

	private:
		std::shared_ptr<std::deque<std::string>> GetLines();

		std::shared_ptr<std::vector<std::shared_ptr<webvtt::Cue>>> _nextCues;
		bool _headRead = false;
		std::shared_ptr<ov::Data> _currentData;
		std::string _lineDelimiter = "\n";
		std::string _header = "WEBVTT";
	};
}