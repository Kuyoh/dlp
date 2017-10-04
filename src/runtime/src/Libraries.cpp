#define BUILD_DLPRT_API
#include "Libraries.hpp"
#include <map>
#include <string>
#include <cstdlib>
#include <dynload.h>

struct Libraries {
	struct Library {
		~Library() { if (pLib) dlFreeLibrary(pLib); }
		void *findSymbol(const char *name) {
			void *&result = symbols[name];
			if (!result) {
				result = dlFindSymbol(pLib, name);
				if (pLib && !result) {
					printf("unable to find symbol '%s' in library '%s'\n", name, libName.c_str());
					exit(1);
				}
			}
			return result;
		}
		DLLib *pLib = nullptr;
		std::string libName;
		std::map<std::string, void*> symbols;
	};

	Library &get(const char *name) {
		Library &result = libs[name];
		if (!result.pLib) {
			result.libName = name;
			result.pLib = dlLoadLibrary(name);
			if (!result.pLib) {
				printf("unable to load library '%s'\n", name);
				exit(1);
			}
		}
		return libs[name];
	}
	std::map<std::string, Library> libs;
} libraries;

extern "C" void *findDynSymbol(const char *libName, const char *symName) {
	return libraries.get(libName).findSymbol(symName);
}