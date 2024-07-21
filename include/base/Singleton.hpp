#pragma once
#include "base/NonCopyable.hpp"

namespace cm {
	template<typename T,typename... Args>
	class Singleton : NonCopyable {
	public:
		static T &instance(Args... args) {
			static T t(args...);
			return t;
		}
	};
}
