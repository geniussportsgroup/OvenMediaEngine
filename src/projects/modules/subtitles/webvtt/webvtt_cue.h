//==============================================================================
//
//  MPEGTS Section
//
//  Created by Getroot
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================

#pragma once

#include <vector>
#include <memory>

namespace webvtt
{
	class Cue
	{
	public:
		Cue();
		~Cue();

		bool Parse(std::string timeInterval, std::string text);

	private:
		std::int64_t CalculateMiliseconds(std::string time);

		std::string _timeIntervalDelimiter = " --> ";
		std::string	_text;
		std::int64_t	_time_start;
		std::int64_t	_time_end;
		std::int64_t	_duration;
	};
}