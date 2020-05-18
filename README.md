# EPL JEMallocPlugin
Apama EPL Plugin for requesting things from the JEMalloc allocator, when in use

## Supported Apama version

This works with Apama 10.5 or later

## Building the plugin

In an Apama command prompt on Linux run:

    mkdir -p $APAMA_WORK/lib
	 g++ -std=c++11 -o $APAMA_WORK/lib/libJEMallocPlugin.so -I$APAMA_HOME/include -L$APAMA_HOME/lib -ljemalloc -lapclient -I. -shared -fPIC JEMallocPlugin.cpp


On Windows run:

    g++ -std=c++11 -o %APAMA_WORK%\lib\JEMallocPlugin.dll -I%APAMA_HOME%\include -L%APAMA_HOME%\lib -ljemalloc -lapclient -I. -shared JEMallocPlugin.cpp

## Running tests

To run the tests for the plugin you will need to use an Apama command prompt to run the tests from within the tests directory:

    pysys run

## Using the JEMallocPlugin

	monitor test
	{
		action onload()
		{
			com.apamax.JEMalloc.flushCurrentThreadCache();
			com.apamax.JEMalloc.purgeArena(0);
		}
	}
