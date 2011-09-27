// dllmain.h : Declaration of module class.

class CGoogleDesktopSolidworksPluginModule : public ATL::CAtlDllModuleT< CGoogleDesktopSolidworksPluginModule >
{
public :
	DECLARE_LIBID(LIBID_GoogleDesktopSolidworksPluginLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GOOGLEDESKTOPSOLIDWORKSPLUGIN, "{A95DC35F-CF18-4083-AE0B-1D4D409F9F70}")
};

extern class CGoogleDesktopSolidworksPluginModule _AtlModule;
