// Copyright 2004-2007 Google Inc.
// All rights reserved.
//

#ifndef COMMON_GOOGLEDESKTOPCOMPONENTREGISTRATION_H__
#define COMMON_GOOGLEDESKTOPCOMPONENTREGISTRATION_H__

// Attempts to register the provided CLSID with GDS
// if num_suffixes is zero does not attempt to register for crawl 
// events
// @param clsid class id of this component, must be unique to this component
// @param title name for the component, displayed to the user, cannot be NULL
// @param description info about the component, displayed to the user, cannot
//    be NULL.
// @param iocn full path to an icon for this component, cannot be NULL
// @param num_suffixes number of pointers in the suffixes array
// @param suffixes file extensions with which this plugin should be registered
HRESULT RegisterIndexingComponentHelper(REFCLSID clsid,
                                        const wchar_t *title, 
                                        const wchar_t *description,
                                        const wchar_t *icon,
                                        int num_suffixes,
                                        const wchar_t *suffixes[]);

// Attempts to register the provided CLSID with GDS as a display component
//    (eg. a sidebar/notification plugin)
// @param clsid class id of this component, must be unique to this component
// @param title name for the component, displayed to the user, cannot be NULL
// @param description info about the component, displayed to the user, cannot
//    be NULL.
// @param iocn full path to an icon for this component, cannot be NULL
// @param shows_notifications flag set to true if this plugin displays
//    notifications. This will list the plugin in the options dialog and
//    allow the user to enable/disable notifications for this plugin
HRESULT RegisterDisplayComponentHelper(REFCLSID clsid,
                                       const wchar_t *title, 
                                       const wchar_t *description,
                                       const wchar_t *icon,
                                       bool shows_notifications);

// Attempts to register the provided CLSID with GDS as a query component.
// @param clsid class id of this component, must be unique to this component
// @param title name for the component, displayed to the user, cannot be NULL
// @param description info about the component, displayed to the user, cannot
//    be NULL.
// @param iocn full path to an icon for this component, cannot be NULL
// @param read_only If 'true', you can do search queries but cannot delete
//    items from the index. A 'false' value gives you both read and write
//    access, allowing you to delete index items.
// @param cookie Registration cookie that is returned. Save this value, as you
//    will have to use it whenever you make a query or remove an item from the
//    Desktop index via the Query API.
HRESULT RegisterQueryComponentHelper(REFCLSID clsid,
                                     const wchar_t *title, 
                                     const wchar_t *description,
                                     const wchar_t *icon,
                                     bool read_only,
                                     long *cookie);

HRESULT UnregisterComponentHelper(REFCLSID clsid);

#endif  // COMMON_GOOGLEDESKTOPCOMPONENTREGISTRATION_H__
