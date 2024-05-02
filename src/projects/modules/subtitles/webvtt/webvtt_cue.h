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
	struct Cue
	{
		std::string	_time_start;
		std::string	_time_end;
		std::string	_text;
	};
}