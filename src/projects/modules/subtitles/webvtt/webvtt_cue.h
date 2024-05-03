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
		std::string _timeIntervalDelimiter = " --> ";
		std::string	_text;
		std::string	_time_start;
		std::string	_time_end;
	};
}