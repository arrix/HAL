/**
 * HAL
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "HAL/JSArray.hpp"
#include "HAL/JSValue.hpp"
#include "HAL/JSString.hpp"
#include "HAL/detail/JSUtil.hpp"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cassert>

namespace HAL {

JSArray::JSArray(const JSContext& js_context, const std::vector<JSValue>& arguments)
		: JSObject(js_context, MakeArray(js_context, arguments)) {
}

JSArray::JSArray(const JSContext& js_context, JSObjectRef js_object_ref)
		: JSObject(js_context, js_object_ref) {
}

JSObjectRef JSArray::MakeArray(const JSContext& js_context, const std::vector<JSValue>& arguments) {
	JSValueRef exception { nullptr };
	JSObjectRef js_object_ref = nullptr;
	if (!arguments.empty()) {
		std::vector<JSValueRef> arguments_array = detail::to_vector(arguments);
		js_object_ref = JSObjectMakeArray(static_cast<JSContextRef>(js_context), arguments_array.size(), &arguments_array[0], &exception);
	} else {
		js_object_ref = JSObjectMakeArray(static_cast<JSContextRef>(js_context), 0, nullptr, &exception);
	}
	
	if (exception) {
		// If this assert fails then we need to JSValueUnprotect
		// js_object_ref.
		assert(!js_object_ref);
		detail::ThrowRuntimeError("JSArray", JSValue(js_context, exception));
	}

	return js_object_ref;
}

uint32_t JSArray::GetLength() const HAL_NOEXCEPT {
	if (!HasProperty("length")) {
		return 0;
	}
	const auto length = GetProperty("length");
	if (!length.IsNumber()) {
		return 0;
	}
	return static_cast<uint32_t>(length);
}

JSArray::operator std::vector<JSValue>() const {
	const auto length = GetLength();
	std::vector<JSValue> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(GetProperty(i));
	}
	return items;
}

JSArray::operator std::vector<bool>() const {
	const auto length = GetLength();
	std::vector<bool> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(static_cast<bool>(GetProperty(i)));
	}
	return items;
}

JSArray::operator std::vector<std::string>() const {
	const auto length = GetLength();
	std::vector<std::string> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(static_cast<std::string>(GetProperty(i)));
	}
	return items;
}

JSArray::operator std::vector<double>() const {
	const auto length = GetLength();
	std::vector<double> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(static_cast<double>(GetProperty(i)));
	}
	return items;
}

JSArray::operator std::vector<int32_t>() const {
	const auto length = GetLength();
	std::vector<int32_t> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(static_cast<int32_t>(GetProperty(i)));
	}
	return items;
}

JSArray::operator std::vector<uint32_t>() const {
	const auto length = GetLength();
	std::vector<uint32_t> items;
	items.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		items.push_back(static_cast<uint32_t>(GetProperty(i)));
	}
	return items;
}


} // namespace HAL {
