//==============================================================================
//
//  MPEGTS Provider
//
//  Created by Getroot
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================

#include "webvtt_provider.h"

#include <modules/physical_port/physical_port_manager.h>
#include <orchestrator/orchestrator.h>

#include "webvtt_application.h"
#include "webvtt_provider_private.h"
#include "webvtt_stream.h"

namespace pvd
{
	static void DumpDataToFile(const std::shared_ptr<ov::Socket> &remote,
							   const ov::SocketAddress &address,
							   const std::shared_ptr<const ov::Data> &data)
	{
		auto file_name = "subtitles.vtt";

		ov::DumpToFile(ov::PathManager::Combine(ov::PathManager::GetAppPath("dump/webvtt"), file_name), data, 0L, true);
	}

	std::shared_ptr<WebVTTProvider> WebVTTProvider::Create(const cfg::Server &server_config, const std::shared_ptr<MediaRouterInterface> &router)
	{
		auto provider = std::make_shared<WebVTTProvider>(server_config, router);
		if (!provider->Start())
		{
			return nullptr;
		}
		return provider;
	}

	WebVTTProvider::WebVTTProvider(const cfg::Server &server_config, const std::shared_ptr<MediaRouterInterface> &router)
		: PushProvider(server_config, router)
	{
		logtd("Created WebVTT Provider module.");
	}

	WebVTTProvider::~WebVTTProvider()
	{
		logti("Terminated WebVTT Provider module.");
	}

	bool WebVTTProvider::Start()
	{
		if (_physical_port_list.empty() == false)
		{
			logtw("RTMP server is already running");
			return false;
		}

		auto server = GetServerConfig();
		const auto &webvtt_config = server.GetBind().GetProviders().GetWebVTT();

		if (webvtt_config.IsParsed() == false)
		{
			logtw("%s is disabled by configuration", GetProviderName());
			return true;
		}

		bool is_configured;
		auto worker_count = webvtt_config.GetWorkerCount(&is_configured);
		worker_count = is_configured ? worker_count : PHYSICAL_PORT_USE_DEFAULT_COUNT;

		std::vector<ov::SocketAddress> webvtt_address_list;

		try
		{
			webvtt_address_list = ov::SocketAddress::Create(server.GetIPList(), static_cast<uint16_t>(webvtt_config.GetPort().GetPort()));
		}
		catch (const ov::Error &e)
		{
			logte("Could not listen for WebVTT: %s", e.What());
			return false;
		}

		if (webvtt_address_list.empty())
		{
			logte("Could not obtain IP list from IP(s): %s, port: %d",
				  ov::String::Join(server.GetIPList(), ", "),
				  static_cast<uint16_t>(webvtt_config.GetPort().GetPort()));

			return false;
		}

		auto port_manager = PhysicalPortManager::GetInstance();
		std::vector<ov::String> webvtt_address_string_list;

		for (const auto &webvtt_address : webvtt_address_list)
		{
			auto physical_port = port_manager->CreatePort("WebVTT", ov::SocketType::Tcp, webvtt_address, worker_count);

			if (physical_port == nullptr)
			{
				logte("Could not initialize physical port for WebVTT server: %s", webvtt_address.ToString().CStr());

				for (auto &physical_port : _physical_port_list)
				{
					physical_port->RemoveObserver(this);
					port_manager->DeletePort(physical_port);
				}
				_physical_port_list.clear();

				return false;
			}

			webvtt_address_string_list.emplace_back(webvtt_address.ToString());

			physical_port->AddObserver(this);
			_physical_port_list.push_back(physical_port);
		}

		logti("%s is listening on %s",
			  GetProviderName(),
			  ov::String::Join(webvtt_address_string_list, ", ").CStr());

		return Provider::Start();
	}

	bool WebVTTProvider::Stop()
	{
		auto port_manager = PhysicalPortManager::GetInstance();

		for (auto &physical_port : _physical_port_list)
		{
			physical_port->RemoveObserver(this);
			port_manager->DeletePort(physical_port);
		}
		_physical_port_list.clear();

		return Provider::Stop();
	}

	bool WebVTTProvider::OnCreateHost(const info::Host &host_info)
	{
		return true;
	}

	bool WebVTTProvider::OnDeleteHost(const info::Host &host_info)
	{
		return true;
	}

	std::shared_ptr<pvd::Application> WebVTTProvider::OnCreateProviderApplication(const info::Application &application_info)
	{
		if (IsModuleAvailable() == false)
		{
			return nullptr;
		}

		return WebVTTApplication::Create(GetSharedPtrAs<pvd::PushProvider>(), application_info);
	}

	bool WebVTTProvider::OnDeleteProviderApplication(const std::shared_ptr<pvd::Application> &application)
	{
		return PushProvider::OnDeleteProviderApplication(application);
	}

	// This function is not called by PhysicalPort when the protocol is udp (MPEGTS/UDP)
	void WebVTTProvider::OnConnected(const std::shared_ptr<ov::Socket> &remote)
	{
		auto channel_id = remote->GetNativeHandle();

		auto stream = WebVTTStream::Create(StreamSourceType::WebVTT, channel_id, remote, GetSharedPtrAs<pvd::PushProvider>());

		logti("A WebVTT	 client has connected from %s", remote->ToString().CStr());

		PushProvider::OnChannelCreated(channel_id, stream);
	}

	void WebVTTProvider::OnDataReceived(const std::shared_ptr<ov::Socket> &remote,
									  const ov::SocketAddress &address,
									  const std::shared_ptr<const ov::Data> &data)
	{
		DumpDataToFile(remote, address, data);

		PushProvider::OnDataReceived(remote->GetNativeHandle(), data);
	}

	// This function is not called by PhysicalPort when the protocol is udp (MPEGTS/UDP)
	void WebVTTProvider::OnDisconnected(const std::shared_ptr<ov::Socket> &remote,
										PhysicalPortDisconnectReason reason,
										const std::shared_ptr<const ov::Error> &error)
	{
		auto channel = GetChannel(remote->GetNativeHandle());
		if (channel == nullptr)
		{
			logte("Failed to find channel to delete stream (remote : %s)", remote->ToString().CStr());
			return;
		}

		logti("The WebVTT client has disconnected: [%s/%s], remote: %s",
			  channel->GetApplicationName(), channel->GetName().CStr(),
			  remote->ToString().CStr());

		PushProvider::OnChannelDeleted(remote->GetNativeHandle());
	}
}  // namespace pvd