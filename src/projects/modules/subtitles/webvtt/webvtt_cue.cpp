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
#include <iomanip>
#include <memory>
#include <sstream>
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
		auto timeIntervalDelimiterIndex = timeInterval.find(_timeIntervalDelimiter);

		if (timeIntervalDelimiterIndex == std::string::npos)
		{
			return false;
		}

		auto startInterval = timeInterval.substr(0, timeIntervalDelimiterIndex);

		auto endIntervalStart = timeIntervalDelimiterIndex + _timeIntervalDelimiter.length();
		auto endInterval = timeInterval.substr(endIntervalStart, timeInterval.length() - endIntervalStart);

		_time_start = CalculateMiliseconds(startInterval);
		_time_end = CalculateMiliseconds(endInterval);
		_duration = _time_end - _time_start;

		return true;
	}
	std::int64_t Cue::CalculateMiliseconds(std::string time)
	{
		std::istringstream iss(time);
		int minutes, seconds;
		char colon;
		double milliseconds;
		iss >> minutes >> colon >> seconds >> colon >> milliseconds;

		return milliseconds + 1000 * seconds + 1000 * minutes * 30;
	}
}  // namespace webvtt