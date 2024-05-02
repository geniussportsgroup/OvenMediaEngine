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

		const std::shared_ptr<webvtt::Cue> GetNextCue();

	private:
		webvtt::Cue	_nextCue;

	};
}