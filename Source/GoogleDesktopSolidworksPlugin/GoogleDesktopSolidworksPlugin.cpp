// GoogleDesktopSolidworksPlugin.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "GoogleDesktopSolidworksPlugin_i.h"
#include "dllmain.h"

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
		return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
  HRESULT hr = _AtlModule.DllRegisterServer();

	const wchar_t *suffixes[] = { L"sldprt", L"sldasm", L"slddrw"};
	hr = RegisterIndexingComponentHelper(CLSID_SolidworksCrawlPlugin, 
				L"SolidWorks Crawler", 
				L"Indexes SolidWorks files", 
				L"PlainWindow.ico", 3, suffixes);

  return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	hr = UnregisterComponentHelper(CLSID_SolidworksCrawlPlugin);
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}


