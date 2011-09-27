// SolidworksCrawlPlugin.cpp : Implementation of CSolidworksCrawlPlugin

#include "stdafx.h"
#include <atlstr.h>
#include <atlsafe.h>
#include <shobjidl.h>
#include <shlguid.h>
#include "SolidworksCrawlPlugin.h"
#include "../common/GoogleDesktopAPI.h"
#include <stdio.h>

#import <mscorlib.tlb> raw_interfaces_only
#import "IFilterFileReader.tlb" no_namespace named_guids


// CSolidworksCrawlPlugin

STDMETHODIMP CSolidworksCrawlPlugin::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_ISolidworksCrawlPlugin
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CSolidworksCrawlPlugin::HandleFile(BSTR full_path_to_file, IDispatch* event_factory)
{
	// Event factory object to handle the passed in pointer from Google Desktop.
	CComPtr<IGoogleDesktopEventFactory> mcEventFactory;

	// Used to create an event object for this file to send back to Google Desktop.
	CComPtr<IDispatch> mcEventDisp;

	HRESULT hr;

	hr  = event_factory->QueryInterface(&mcEventFactory); // Get the event factory
	if (FAILED(hr))
		return Error(L"No event factory", IID_ISolidworksCrawlPlugin, hr);

	// Replace "Google.Desktop.TextFile" with the appropriate schema for your file.
	hr = mcEventFactory->CreateEvent(CComBSTR(CLSID_SolidworksCrawlPlugin),
		CComBSTR(L"Google.Desktop.TextFile"), &mcEventDisp);
	if (FAILED(hr))
		return Error(L"Unable to create event", IID_ISolidworksCrawlPlugin, hr);

	CComQIPtr<IGoogleDesktopEvent> mcEvent(mcEventDisp);

	ATLASSERT(mcEventDisp && mcEvent);
	if (mcEvent == NULL)
		return Error(L"Event does not implement IGoogleDesktopEvent", IID_ISolidworksCrawlPlugin, E_UNEXPECTED);

	// Get indexable content from the file.
	// Set the event object's properties.
	// Send the event object to Google Desktop.

	WIN32_FIND_DATA wfd;
	if (!GetFileAttributesEx(full_path_to_file, GetFileExInfoStandard, &wfd))
		return HRESULT_FROM_WIN32(::GetLastError());

	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&wfd.ftCreationTime, &systemTime);

	// convert the date to the variant format
	double varDate;
	SystemTimeToVariantTime(&systemTime, &varDate);

	hr = mcEvent->AddProperty(CComBSTR(L"uri"), CComVariant(full_path_to_file));
	if (FAILED(hr))
		return Error(L"AddProperty uri failed", IID_ISolidworksCrawlPlugin, hr);

	hr = mcEvent->AddProperty(CComBSTR(L"last_modified_time"), CComVariant(varDate, VT_DATE));
	if (FAILED(hr))
		return Error(L"AddProperty last_modified_time failed", IID_ISolidworksCrawlPlugin, hr);

	hr = mcEvent->AddProperty(CComBSTR(L"format"), CComVariant("text/plain"));
	if (FAILED(hr))
		return Error(L"AddProperty format failed", IID_ISolidworksCrawlPlugin, hr);

	// Initialize COM and create an instance of the InterfaceImplementation class:
	IManagedInterface *cpi = NULL;
	hr = CoCreateInstance(CLSID_IFilterFileReader,
		NULL, CLSCTX_INPROC_SERVER,
		IID_IManagedInterface, reinterpret_cast<void**>(&cpi));
	if (FAILED(hr))
	{
		return Error(L"Couldn't create the instanced", IID_ISolidworksCrawlPlugin, hr);
	}

	hr = cpi->loadFile(full_path_to_file);
	if (FAILED(hr))
	{
		return Error(L"Couldn't loadFile", IID_ISolidworksCrawlPlugin, hr);
	}

	hr = mcEvent->AddProperty(CComBSTR(L"content"), CComVariant(cpi->readFile().GetBSTR()));
	if (FAILED(hr))
		return Error(L"AddProperty content failed", IID_ISolidworksCrawlPlugin, hr);

	cpi->Release();
	cpi = NULL;

	hr = mcEvent->Send(EventFlagIndexable);
	if (FAILED(hr))
		return Error(L"Send failed", IID_ISolidworksCrawlPlugin, hr);

	return hr;
}
