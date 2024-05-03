//==============================================================================
//
//  MPEGTS Section
//
//  Created by Getroot
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================

#pragma once

#include "webvtt_cue.h"

#include <chrono>
#include <memory>
#include <vector>

namespace webvtt
{

	webvtt::Cue::Cue()
	{
	}
	Cue::~Cue()
	{
	}
	bool Cue::Parse(const std::string timeInterval, const std::string text)
	{
		std::localtime()

		auto timeIntervalDelimiterIndex = timeInterval.find(_timeIntervalDelimiter);

		if (timeIntervalDelimiterIndex == std::string::npos)
		{
			return false;
		}

		auto startInterval = timeInterval.substr(0, timeIntervalDelimiterIndex);

		auto endIntervalStart = timeIntervalDelimiterIndex + _timeIntervalDelimiter.length();
		auto endInterval = timeInterval.substr(endIntervalStart, timeInterval.length() - endIntervalStart);

		return false;
	}
}  // namespace webvtt