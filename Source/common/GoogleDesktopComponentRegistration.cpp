// Copyright 2004-2007 Google Inc.
// All rights reserved.
// 

#include "stdafx.h"
#include <atlsafe.h>

#include "GoogleDesktopAPI.h"
#include "GoogleDesktopComponentRegistration.h"

HRESULT RegisterIndexingComponentHelper(REFCLSID clsid,
                                        const wchar_t *title, 
                                        const wchar_t *description,
                                        const wchar_t *icon,
                                        int num_suffixes,
                                        const wchar_t *suffixes[]) { 
  ATLASSERT(title != NULL);
  ATLASSERT(description != NULL);
  ATLASSERT(icon != NULL);
  if (!title || !description || !icon)
    return E_POINTER;

  CComPtr<IGoogleDesktopRegistrar> spRegistrar;
  HRESULT hr;
  
  hr = spRegistrar.CoCreateInstance(CLSID_GoogleDesktopRegistrar);
  
  if (SUCCEEDED(hr)) {
    ATLASSERT(spRegistrar != NULL);
    
    // Component description is 6 strings
    CComSafeArray<VARIANT> arr_descr(6);

    arr_descr.SetAt(0, CComVariant(L"Title"));
    arr_descr.SetAt(1, CComVariant(title));
    arr_descr.SetAt(2, CComVariant(L"Description"));
    arr_descr.SetAt(3, CComVariant(description));
    arr_descr.SetAt(4, CComVariant(L"Icon"));
    arr_descr.SetAt(5, CComVariant(icon));

    // our CLSID in string format
    CComBSTR our_clsid(clsid);
    
    // Wrap description array in variant
    CComVariant descr(arr_descr.m_psa);

    // and register
    hr = spRegistrar->StartComponentRegistration(our_clsid, descr);
    if (FAILED(hr))
      return hr;

    // success, now register as an indexing component.
    CComPtr<IGoogleDesktopRegisterIndexingPlugin> spRegistration;

    CComBSTR indexing_registrar_progid("GoogleDesktop.IndexingRegistration");
    hr = spRegistrar->GetRegistrationInterface(indexing_registrar_progid,
      reinterpret_cast<IUnknown**>(&spRegistration));
    ATLASSERT(FAILED(hr) || spRegistration);

    // register the plugin and extensions (if any)
    if (SUCCEEDED(hr)) {
      hr = spRegistration->RegisterIndexingPlugin(NULL);

      for (int i = 0; SUCCEEDED(hr) && i < num_suffixes; ++i) 
        hr = spRegistration->RegisterIndexingPlugin(CComBSTR(suffixes[i]));

      if (SUCCEEDED(hr)) {
        hr = spRegistrar->FinishComponentRegistration();
      }
    }
      
    // revoke our registration in case any of any failure
    if (FAILED(hr)) {
      // ignore the error - we've already failed
      HRESULT ignore;      
      ignore = spRegistrar->UnregisterComponent(our_clsid);
    }    
  }

  return hr;
}

HRESULT RegisterDisplayComponentHelper(REFCLSID clsid,
                                       const wchar_t *title, 
                                       const wchar_t *description,
                                       const wchar_t *icon,
                                       bool shows_notifications) { 
  ATLASSERT(title != NULL);
  ATLASSERT(description != NULL);
  ATLASSERT(icon != NULL);
  if (!title || !description || !icon)
    return E_POINTER;

  CComPtr<IGoogleDesktopRegistrar> spRegistrar;
  HRESULT hr;
  
  hr = spRegistrar.CoCreateInstance(CLSID_GoogleDesktopRegistrar);
  
  if (SUCCEEDED(hr)) {
    ATLASSERT(spRegistrar != NULL);
    
    // Component description is 6 strings
    CComSafeArray<VARIANT> arr_descr(6);

    arr_descr.SetAt(0, CComVariant(L"Title"));
    arr_descr.SetAt(1, CComVariant(title));
    arr_descr.SetAt(2, CComVariant(L"Description"));
    arr_descr.SetAt(3, CComVariant(description));
    arr_descr.SetAt(4, CComVariant(L"Icon"));
    arr_descr.SetAt(5, CComVariant(icon));

    // our CLSID in string format
    CComBSTR our_clsid(clsid);
    
    // Wrap description array in variant
    CComVariant descr(arr_descr.m_psa);

    // and register
    hr = spRegistrar->StartComponentRegistration(our_clsid, descr);
    if (FAILED(hr))
      return hr;

    // success, now register as a display component.
    // This two step registration allows the same plugin to implement multiple
    // components
    CComPtr<IGoogleDesktopRegisterDisplayPlugin> spRegistration;

    CComBSTR sidebar_registrar_progid("GoogleDesktop.DisplayPluginRegistration");
    hr = spRegistrar->GetRegistrationInterface(sidebar_registrar_progid,
      reinterpret_cast<IUnknown**>(&spRegistration));
    ATLASSERT(FAILED(hr) || spRegistration);

    if (SUCCEEDED(hr)) {
      hr = spRegistration->RegisterPlugin(our_clsid, (shows_notifications) ?
        VARIANT_TRUE : VARIANT_FALSE);

      if (SUCCEEDED(hr)) {
        hr = spRegistrar->FinishComponentRegistration();
      }
    }
      
    // revoke our registration in case any of any failure
    if (FAILED(hr)) {
      // ignore the error - we've already failed
      HRESULT ignore;      
      ignore = spRegistrar->UnregisterComponent(our_clsid);
    }    
  }

  return hr;
}

HRESULT RegisterQueryComponentHelper(REFCLSID clsid,
                                     const wchar_t *title,
                                     const wchar_t *description,
                                     const wchar_t *icon,
                                     bool read_only,
                                     long *cookie) {
  CoInitialize(NULL);
  ATLASSERT(title != NULL);
  ATLASSERT(description != NULL);
  ATLASSERT(icon != NULL);
  if (!title || !description || !icon || !cookie)
    return E_POINTER;

  CComPtr<IGoogleDesktopRegistrar> spRegistrar;
  HRESULT hr;

  hr = spRegistrar.CoCreateInstance(CLSID_GoogleDesktopRegistrar);
  
  if (SUCCEEDED(hr)) {
    ATLASSERT(spRegistrar != NULL);
    
    // Component description is 6 strings
    CComSafeArray<VARIANT> arr_descr(6);

    arr_descr.SetAt(0, CComVariant(L"Title"));
    arr_descr.SetAt(1, CComVariant(title));
    arr_descr.SetAt(2, CComVariant(L"Description"));
    arr_descr.SetAt(3, CComVariant(description));
    arr_descr.SetAt(4, CComVariant(L"Icon"));
    arr_descr.SetAt(5, CComVariant(icon));

    // our CLSID in string format
    CComBSTR our_clsid(clsid);
    
    // Wrap description array in variant
    CComVariant descr(arr_descr.m_psa);

    // and register
    hr = spRegistrar->StartComponentRegistration(our_clsid, descr);
    if (FAILED(hr)) {
      return hr;
    }

    // success, now register as a query component.
    CComPtr<IGoogleDesktopRegisterQueryPlugin> spRegistration;

    CComBSTR query_registrar_progid("GoogleDesktop.QueryRegistration");
    hr = spRegistrar->GetRegistrationInterface(query_registrar_progid,
      reinterpret_cast<IUnknown**>(&spRegistration));
    ATLASSERT(FAILED(hr) || spRegistration);

    if (SUCCEEDED(hr)) {
      hr = spRegistration->RegisterPlugin(
              our_clsid, (read_only ? VARIANT_TRUE : VARIANT_FALSE), cookie);
      if (SUCCEEDED(hr)) {
        hr = spRegistrar->FinishComponentRegistration();
      }
    }

    // revoke our registration in case any of any failure
    if (FAILED(hr)) {
      // ignore the error - we've already failed
      HRESULT ignore;      
      ignore = spRegistrar->UnregisterComponent(our_clsid);
      *cookie = 0;
    }    
  }

  return hr;
}

HRESULT UnregisterComponentHelper(REFCLSID clsid) {
  CComPtr<IGoogleDesktopRegistrar> spRegistrar;
  HRESULT hr;
  
  hr = spRegistrar.CoCreateInstance(CLSID_GoogleDesktopRegistrar);
  
  if (SUCCEEDED(hr)) { 
    // our CLSID in string format
    CComBSTR bstrClsid(clsid);
  
    hr = spRegistrar->UnregisterComponent(bstrClsid);
  }
  
  return hr;
}
