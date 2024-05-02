//==============================================================================
//
//  WebVTT Application
//
//  Created by Getroot
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================

#include "webvtt_application.h"
#include "webvtt_stream.h"
#include "webvtt_provider_private.h"

#include "base/provider/push_provider/application.h"
#include "base/info/stream.h"

namespace pvd
{
	std::shared_ptr<WebVTTApplication> WebVTTApplication::Create(const std::shared_ptr<PushProvider> &provider, const info::Application &application_info)
	{
		auto application = std::make_shared<WebVTTApplication>(provider, application_info);
		application->Start();
		return application;
	}

	WebVTTApplication::WebVTTApplication(const std::shared_ptr<PushProvider> &provider, const info::Application &application_info)
		: PushApplication(provider, application_info)
	{
	}
}