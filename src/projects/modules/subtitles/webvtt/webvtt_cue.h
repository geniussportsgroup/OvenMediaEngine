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
		char*	_time_start;
		char*	_time_end;
		char*	_text;
	};
}