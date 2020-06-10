/**
 * Title:        JEMallocPlugin.cpp
 * Description:  JEMalloc EPL Plugin
 * Copyright (c) 2020 Software AG, Darmstadt, Germany and/or its licensors
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
#include <fstream>

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

	/** Export all the methods to EPL. */
	static void initialize(base_plugin_t::method_data_t &md)
	{
		md.registerMethod<decltype(&JEMallocPlugin::mallctl), &JEMallocPlugin::mallctl>("mallctl");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getint), &JEMallocPlugin::mallctl_getint>("mallctl_getint");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getbool), &JEMallocPlugin::mallctl_getbool>("mallctl_getbool");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getsize), &JEMallocPlugin::mallctl_getsize>("mallctl_getsize");
		md.registerMethod<decltype(&JEMallocPlugin::mallctl_getunsigned), &JEMallocPlugin::mallctl_getunsigned>("mallctl_getunsigned");
		md.registerMethod<decltype(&JEMallocPlugin::updateepoch), &JEMallocPlugin::updateepoch>("updateepoch");
		md.registerMethod<decltype(&JEMallocPlugin::readfile), &JEMallocPlugin::readfile>("readfile");
	}

	/** Read the contents of a file from disk. Needed as a hack for calling malloc_stats management request. */
	std::string readfile(const char *path)
	{
		std::ifstream ifs(path, std::ios::in | std::ios::binary | std::ios::ate);
		auto size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		std::vector<char> bytes(size);
		ifs.read(bytes.data(), size);
		return std::string(bytes.data(), size);
	}

	/** Call a malloc command which returns an unsigned. */
	int64_t mallctl_getunsigned(const char *command)
	{
		return mallctl_get<unsigned>(command);
	}
	/** Call a malloc command which returns a bool. */
	bool mallctl_getbool(const char *command)
	{
		return mallctl_get<bool>(command);
	}
	/** Call a malloc command which returns a size_t. */
	int64_t mallctl_getsize(const char *command)
	{
		return mallctl_get<ssize_t>(command);
	}
	/** Call a malloc command which returns an int64_t. */
	int64_t mallctl_getint(const char *command)
	{
		return mallctl_get<int64_t>(command);
	}
	/** Templated get implementation for any size. */
	template<typename T> T mallctl_get(const char *command)
	{
		logger.debug("Executing malloc get: %s", command);
		T rv = T();
		size_t rvsize = sizeof(T);
		je_mallctl(command, &rv, &rvsize, nullptr, 0);
		return rv;
	}

	/** Update the epoch. Must be called before getting any stats. */
	void updateepoch()
	{
		logger.debug("Executing malloc command to update epoch");
		uint64_t epoch;
		size_t epochsize = sizeof(uint64_t);
		je_mallctl("epoch", &epoch, &epochsize, nullptr, 0);
		++epoch;
		je_mallctl("epoch", &epoch, &epochsize, &epoch, epochsize);
	}

	/** Call a malloc command which doesn't return anything. */
	void mallctl(const char *command)
	{
		logger.info("Executing malloc command: %s", command);
		je_mallctl(command, nullptr, nullptr, nullptr, 0);
	}

};

/// Export this plugin
APAMA_DECLARE_EPL_PLUGIN(JEMallocPlugin)

}}
