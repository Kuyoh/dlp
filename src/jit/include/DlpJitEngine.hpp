#ifndef DLP_JIT_DLPJITENGINE_HPP
#define DLP_JIT_DLPJITENGINE_HPP

#ifdef _WIN32
#  ifdef DLP_BUILD_JIT
#    define DLP_JIT_API __declspec(dllexport)
#  else
#    define DLP_JIT_API __declspec(dllimport)
#  endif
#else
#  define DLP_JIT_API
#endif

class DLP_JIT_API DlpJitEngine {
public:
	DlpJitEngine();
	virtual ~DlpJitEngine();

	void add(const char *filename);
	void run();

private:
	struct Data;
	Data *data;
};
#endif