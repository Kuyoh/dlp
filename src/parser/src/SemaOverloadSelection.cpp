#include "SemaOverloadSelection.hpp"

using namespace dlp::sema;

// TODO: consider inter-dependent lookup of function calls and arguments
//  i.e. support C++ style ADL and enum definition usage without having to specify the name of the enum.
int dlp::sema::selectOverload(std::vector<Type*> &argTypes, std::vector<FunctionType*> &overloads) {

	// we need information on available conversion ops -> implement this in translation context instead???
	return 0;
}

/*
string :: struct {
	data : u8*;

	implicit_convert<u8*> :: () {
		return data;
	}
};

// we do not want names for operator overloads -> it may make sense to not name conversion operators as well...
implicit_convert :: (in: string) -> (out: u8*) {
	return in.data;
}

convert<string> :: (in: )

// default pointer types:
// ptr<T> = T*
// shared_ptr
// unique_ptr
// deferred_ptr -> needs associated heap

// argument intention types:
// ref<T>
// const_ref<T>
// retain<T>
// copy<T>
// move<T>

// what about thread safety???

using shared<T> = std::shared_ptr<T>;
using unique<T> = std::unique_ptr<T>;
using weak<T> = std::weak_ptr<T>;
using deferred<T> = ...::deferred_ptr<T>;

template <typename T>
struct ref {
	ref(T *ptr) noexcept : ptr(ptr) {}
	ref(ref<T> ref) noexcept : ptr(ref) {}
	ref(shared<T> &ptr) noexcept : ptr(*ptr) {}
	ref(weak<T> &ptr) : ptr(ptr.get()) {}
	ref(unique<T> &ptr) noexcept : ptr(*ptr) {}
	operator T*() noexcept { return ptr; }
protected:
	T *ptr;
};

template <typename T>
struct const_ref {
	const_ref(const T *ptr) noexcept : ptr(ptr) {}
	
	const_ref(ref<T> ref) noexcept : ptr(ref) {}
	const_ref(const_ref<T> ref) noexcept : ptr(ref) {}
	const_ref(const shared<T> &ptr) noexcept : ptr(*ptr) {}
	const_ref(const weak<T> &ptr) : ptr(ptr.get()) {}
	const_ref(const unique<T> &ptr) noexcept : ptr(*ptr) {}

	const_ref(ref<const T> ref) noexcept : ptr(ref) {}
	const_ref(const_ref<const T> ref) noexcept : ptr(ref) {}
	const_ref(const shared<const T> &ptr) noexcept : ptr(*ptr) {}
	const_ref(const weak<const T> &ptr) : ptr(ptr.get()) {}
	const_ref(const unique<const T> &ptr) noexcept : ptr(*ptr) {}
	operator const T*() const noexcept { return ptr; }
protected:
	const T *ptr;
};

template <typename T>
struct retain { // if sizeof(T) < 32
	T value;
	retain(unique<T> &&ptr) : value(std::move(*ptr)) {}
	retain(shared<T> &&ptr) : value(std::move(*ptr)) {}
	retain(ref<T> &&ptr) : value(std::move(*ptr)) {
		ptr.release();
	}
};

template <typename T>
struct retain_ptr {

};


*/
