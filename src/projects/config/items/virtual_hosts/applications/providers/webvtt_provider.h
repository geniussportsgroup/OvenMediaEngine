//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2019 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include "provider.h"
#include "event_generator/event_generator.h"
#include "options/use_incoming_ts.h"

namespace cfg
{
	namespace vhost
	{
		namespace app
		{
			namespace pvd
			{
				struct WebVTTProvider : public Provider, public UseIncomingTimestamp
				{
				protected:
					bool _is_dump_to_file = true;
				public:
					ProviderType GetType() const override
					{
						return ProviderType::WebVTT;
					}

					CFG_DECLARE_CONST_REF_GETTER_OF(IsDumpToFile, _is_dump_to_file)
					
				protected:
					void MakeList() override
					{
						Provider::MakeList();

						Register<Optional>("DumpToFile", &_is_dump_to_file);
						Register<Optional>("UseIncomingTimestamp", &_use_incoming_timestamp);
					}
				};
			}  // namespace pvd
		} // namespace app
	} // namespace vhost
}  // namespace cfg