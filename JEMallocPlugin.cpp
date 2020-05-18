/**
 * Title:        JEMallocPlugin.cpp
 * Description:  JEMalloc EPL Plugin
 * Copyright (c) 2020 Software AG, Darmstadt, Germany andor its licensors
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 * http:/www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#define __STDC_FORMAT_MACROS 1
#include <epl_plugin.hpp>
#include <inttypes.h>

using namespace com::apama::epl;

extern "C" {
	int je_mallctl(const char *name,
			void *oldp,
			size_t *oldlenp,
			void *newp,
			size_t newlen);
}

namespace com {
namespace apamax {

/**
 * This plugin allows invoking the jemalloc API to force collections
 */
class JEMallocPlugin: public EPLPlugin<JEMallocPlugin>
{
public:
	JEMallocPlugin() : base_plugin_t("JEMallocPlugin")
	{
	}
	static void initialize(base_plugin_t::method_data_t &md)
	{
		md.registerMethod<decltype(&JEMallocPlugin::mallctl), &JEMallocPlugin::mallctl>("mallctl");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getint), &JEMallocPlugin::mallctl_getint>("mallctl_getint");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getbool), &JEMallocPlugin::mallctl_getbool>("mallctl_getbool");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getsize), &JEMallocPlugin::mallctl_getsize>("mallctl_getsize");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getunsigned), &JEMallocPlugin::mallctl_getunsigned>("mallctl_getunsigned");
	}

	int64_t mallctl_getunsigned(const char *command)
	{
		return mallctl_get<unsigned>(command);
	}
	bool mallctl_getbool(const char *command)
	{
		return mallctl_get<bool>(command);
	}
	int64_t mallctl_getsize(const char *command)
	{
		return mallctl_get<ssize_t>(command);
	}
	int64_t mallctl_getint(const char *command)
	{
		return mallctl_get<int64_t>(command);
	}
	template<typename T> T mallctl_get(const char *command)
	{
		logger.info("Executing malloc command: %s", command);
		T rv = T();
		size_t rvsize = sizeof(T);
		je_mallctl(command, &rv, &rvsize, nullptr, 0);
		return rv;
	}

	void mallctl(const char *command)
	{
		logger.info("Executing malloc command: %s", command);
		je_mallctl(command, nullptr, nullptr, nullptr, 0);
	}

};

/// Export this plugin
APAMA_DECLARE_EPL_PLUGIN(JEMallocPlugin)

}}
