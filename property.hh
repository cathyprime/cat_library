#pragma once

/*
 * property.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 11-11-2024
 */

#include <functional> // TODO: replace with cat equivalent

namespace cat { namespace property {

template<typename T> class Property
{
	using SetterLambda = std::function<void(const T&)>;
	using SetterType = std::function<void(T, SetterLambda&)>;
	using GetterType = std::function<T(const T&)>;

  private:
	T backing_field;

	SetterLambda setter_lambda;
	SetterType m_setter;
	GetterType m_getter;

#define set_lambda [this](const T& v) { this->backing_field = v; }

  public:
	Property(T value = T(), SetterType setter = nullptr, GetterType getter = nullptr)
		: m_getter(getter)
	{
		backing_field = value;
		if (setter != nullptr) {
			m_setter = setter;
			setter_lambda = set_lambda;
		}
	}
#undef set_lambda
	Property(Property&& other) noexcept
		: backing_field(std::move(other.backing_field))
		, setter_lambda(std::move(other.setter_lambda))
		, m_setter(std::move(other.m_setter))
		, m_getter(std::move(other.m_getter))
	{}

	Property& operator=(Property&& other) noexcept
	{
		if (this != &other) {
			backing_field = std::move(other.backing_field);
			setter_lambda = std::move(other.setter_lambda);
			m_setter = std::move(other.m_setter);
			m_getter = std::move(other.m_getter);
		}
		return *this;
	}

	operator T() const
	{
		if (m_getter) return m_getter(backing_field);
		return backing_field;
	}

	void operator=(T value)
	{
		if (m_setter)
			m_setter(value, setter_lambda);
		else {
			backing_field = value;
		}
	}

	T* operator->()
	{
		return &backing_field;
	}

	const T* operator->() const
	{
		return &backing_field;
	}
};

}} // namespace cat::property
