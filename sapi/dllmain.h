// dllmain.h : Declaration of module class.

class ChtssapiModule : public ATL::CAtlDllModuleT< ChtssapiModule > {
   public :
      DECLARE_LIBID(LIBID_htssapiLib)
      DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HTSSAPI, "{65A0151C-F352-4243-9267-D5815C631A29}")
};

extern class ChtssapiModule _AtlModule;
