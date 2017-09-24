#include "Builtin.hpp"
#include <unordered_map>

using namespace dlp::sema;

auto getEntities() {
	std::unordered_map<std::string, std::shared_ptr<Entity>> entities = {
		{ "bool",  std::make_shared<BoolType>() },
		{ "u8",  std::make_shared<IntType>(Type::B8 , false) },
		{ "u16", std::make_shared<IntType>(Type::B16, false) },
		{ "u32", std::make_shared<IntType>(Type::B32, false) },
		{ "u64", std::make_shared<IntType>(Type::B64, false) },
		{ "s8",  std::make_shared<IntType>(Type::B8 , true) },
		{ "s16", std::make_shared<IntType>(Type::B16, true) },
		{ "u32", std::make_shared<IntType>(Type::B32, true) },
		{ "u64", std::make_shared<IntType>(Type::B64, true) },
		{ "f32", std::make_shared<FloatType>(Type::B32) },
		{ "f64", std::make_shared<FloatType>(Type::B64) },
		{ "string", std::make_shared<StringType>() },
	};
	// TODO: add builtin functions
	return std::move(entities);
}

Entity *dlp::sema::resolveBuiltin(const std::string &name) {
	static auto entities = getEntities();
	auto it = entities.find(name);
	if (it != entities.end())
		return it->second.get();
	return nullptr;
}