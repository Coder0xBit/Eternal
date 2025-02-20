#pragma once

#include "PrivateImplementation.h"

#include <utility>

namespace utils {
	template<typename T>
	PrivateImplementation<T>::PrivateImplementation() noexcept :mImpl(new T) {}

	template<typename T>
	template<typename ...Args>
	PrivateImplementation<T>::PrivateImplementation(Args&& ...args) noexcept : mImpl(new T(std::forward<Args>(args)...)) {}

	template<typename T>
	PrivateImplementation<T>::~PrivateImplementation() noexcept
	{
		delete mImpl;
	}

	template<typename T>
	PrivateImplementation<T>::PrivateImplementation(const PrivateImplementation& rhs) noexcept : mImpl(new T(*rhs.mImpl)) {}

	template<typename T>
	PrivateImplementation<T>& PrivateImplementation<T>::operator=(const PrivateImplementation<T>& rhs) noexcept
	{
		if (this != &rhs) {
			*mImpl = *rhs.mImpl;
		}
		return*this;
	}

}