# EPL JEMallocPlugin
Apama EPL Plugin for requesting things from the JEMalloc allocator, when in use

## Supported Apama version

This works with Apama 10.5 or later and only on linux

## Building the plugin

In an Apama command prompt on Linux run:

    mkdir -p $APAMA_WORK/lib
	 g++ -std=c++11 -o $APAMA_WORK/lib/libJEMallocPlugin.so -I$APAMA_HOME/include -L$APAMA_HOME/lib -ljemalloc -lapclient -I. -shared -fPIC JEMallocPlugin.cpp

## Running tests

To run the tests for the plugin you will need to use an Apama command prompt to run the tests from within the tests directory:

    pysys run

## Doing explicit JEMalloc requests

The file `JEMalloc.mon` provides access to explicit JEMalloc requests, eg:

	monitor test
	{
		action onload()
		{
			com.apamax.JEMalloc.flushCurrentThreadCache();
			com.apamax.JEMalloc.purgeArena(0);
		}
	}

## Getting memory status lines

The file `JEMallocStatus.mon` will provide periodic status lines with additional memory statistics from both JEMalloc and the platform allocator. To use it you will need to have injected `JEMalloc.mon` from this directory and `ManagementImpl.mon`, `Management.mon` and `TimeFormatEvents.mon` from your Apama installation. You can do this with an initialization list, for example:

	correlator:
		initialization:
			list:
				- ${APAMA_HOME}/monitors/ManagementImpl.mon
				- ${APAMA_HOME}/monitors/Management.mon
				- ${APAMA_HOME}/monitors/TimeFormatEvents.mon
				- ${PARENT_DIR}/JEMalloc.mon
				- ${PARENT_DIR}/JEMallocStatus.mon

To use this you must also set one of the following environment variables before the correlator starts:

- `AP_ALLOCATOR=jemalloc` - enable the JEmalloc allocator
- `AP_TRACKMEMORY=true` - track stats for the platform allocator

You can also configure the status line via two correlator properties:

- `MEMORY_STATUS_INTERVAL_SECS` - the number of seconds between printing each status line (default: 5)
- `MEMORY_STATUS_TEMP_FILE` - the temporary file to use for gathering stats for the platform allocator (default: `/tmp/JEMallocStats.tmp.file.txt`)

The status line gives values in megabytes (note: this is in contrast to the Apama status line which uses kilobytes). The status line contains the following items (depending whether they are enabled):

- `jeall` - (JEMalloc) Total number of bytes allocated by the application.
- `jeres` - (JEMalloc) Maximum number of bytes in physically resident data pages mapped by the allocator.
- `jeact` - (JEMalloc) Total number of bytes in active pages allocated by the application.
- `jemap` - (JEMalloc) Total number of bytes in active extents mapped by the allocator.
- `jeret` - (JEMalloc) Total number of bytes in virtual memory mappings that were retained rather than being returned to the operating system.
- `jeefficiency` - (JEMalloc) Percentage of max(res, map) which is allocated by the application.
- `use` - (System) In-use bytes.
- `sys` - (System) System bytes.
- `max` - (System) System max.
- `cur` - (System) System current.
- `tot` - (System) Aspace total.
- `efficiency` - (System) Percentage of current which is in-use.

## API documentation

API documentation can be found here: [API documentation](https://mjj29.github.io/apama-jemalloc-plugin/)
