/**
 * JavaScriptCoreCPP
 * Author: Matthew D. Langston
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _JAVASCRIPTCORECPP_RAII_JSNATIVEOBJECTBUILDER_HPP_
#define _JAVASCRIPTCORECPP_RAII_JSNATIVEOBJECTBUILDER_HPP_


#include "JavaScriptCoreCPP/RAII/JSNativeObject.hpp"
#include "JavaScriptCoreCPP/RAII/JSNativeObjectDefinition.hpp"
#include "JavaScriptCoreCPP/RAII/JSContext.hpp"
#include "JavaScriptCoreCPP/RAII/JSClass.hpp"
#include <unordered_set>
#include <memory>
#include <JavaScriptCore/JavaScript.h>

namespace JavaScriptCoreCPP { namespace RAII {

/*!
  @class

  @abstract This is a builder that creates JSNativeObject instances
  that represent a custom JavaScript object backed by a C++ class for
  some of its functionality. All properties on this builder are
  optional, and all callbacks may be nullptr.

  @discussion Adding JSNativeObjectFunctionPropertyCallback and
  JSNativeObjectValuePropertyCallback instances to a
  JSNativeObjectBuilder are the simplest and most efficient means for
  vending custom properties since they autmatically service requests
  like get_property_callback, set_property_callback, and
  get_property_names_callback. The other property access callbacks are
  required only to implement unusual properties, like array indexes,
  whose names are not known at compile-time.
  
  Standard JavaScript practice calls for storing function objects in
  prototypes so that they can be shared. The instances of
  JSNativeObject created by a JSNativeObjectBuilder follows this
  idiom, instantiating objects with a shared, automatically generated
  prototype containing the class's properties.

  To override this, the JSNativeObjectAttributes::NoAutomaticPrototype
  attribute can be set which specifies that a JSNativeObject should
  not automatically generate such a prototype so that an instance has
  only the default object prototype and a copy of the properties.
  
  Setting any callback to nullptr specifies that the default object
  callback should substitute, except in the case of
  has_property_callback, where it specifies that
  get_property_names_callback should substitute.
*/
template<typename T>
class JSNativeObjectBuilder final {

 public:
	
	JSNativeObjectBuilder(const JSContext& js_context) : js_context_(js_context) {
	}
	
	JSNativeObjectBuilder() = delete;;
	~JSNativeObjectBuilder() = default;
	
	JSNativeObjectBuilder(const JSNativeObjectBuilder& rhs) = default;
	JSNativeObjectBuilder(JSNativeObjectBuilder&& rhs) = default;
	
	JSNativeObjectBuilder& operator=(const JSNativeObjectBuilder& rhs) = default;
	JSNativeObjectBuilder& operator=(JSNativeObjectBuilder&& rhs) = default;

	/*!
	  @method
	  
	  @abstract Return the JSContext in which the JSNativeObject is
	  created.
	  
	  @result The JSContext in which the JSNativeObject is created.
	*/
	JSContext get_js_context() const {
		return js_context_;
	}
	
	/*!
	  @method
	  
	  @abstract Set the JSContext in which the JSNativeObject is
	  created.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_js_context(const JSContext& js_context_) {
		js_context_ = js_context;
		return *this;
	}

	/*!
	  @method
	  
	  @abstract Return the class's name.
	  
	  @result The class's name.
	*/
	JSString get_class_name() const {
		return class_name_;
	}
	
	/*!
	  @method
	  
	  @abstract Set the class's name.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_class_name(const JSString& class_name) {
		class_name_ = class_name;
		return *this;
	}
	
	/*!
	  @method
	  
	  @abstract Return the JSNativeObjectAttributes that describe the
	  characteristics of JSNativeObject created by this builder.
	  
	  @result The JSNativeObjectAttributes that describe the
	  characteristics of the JSNativeObject created by this builder.
	*/
	std::unordered_set<JSNativeObjectAttributes> get_attributes() const {
		return attributes_;
	}

	/*!
	  @method
	  
	  @abstract Set the JSNativeObjectAttributes that describe the
	  characteristics of the JSNativeObject created by this builder.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_attributes(const std::unordered_set<JSNativeObjectAttributes>& attributes) {
		attributes_ = attributes;
		return *this;
	}
	
	/*!
	  @method
	  
	  @abstract Return the parent of the JSNativeObject. A nullptr means
	  to use the default object class.
	  
	  @result The parent of the the JSNativeObject created by this
	  builder.
	*/
	std::shared_ptr<JSNativeObject<T>> get_parent_ptr() const {
		return parent_ptr_;
	}

	/*!
	  @method
	  
	  @abstract Set the parent of the JSNativeObject created by this
	  builder. A nullptr means to use the default object class.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_parent_ptr(const std::shared_ptr<JSNativeObject<T>>& parent_ptr) {
		parent_ptr_ = parent_ptr;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the current set of
	  JSNativeObjectValuePropertyCallbacks that describe the
	  JSNativeObject's value properties.
  
	  @result The current set of of JSNativeObjectValuePropertyCallbacks
	  that describe the JSNativeObject's value properties.
	*/
	std::unordered_set<JSNativeObjectValuePropertyCallback<T>> get_value_property_callbacks() const {
		return value_property_callbacks_;
	}
	
	/*!
	  @method

	  @abstract Replace the current set of of
	  JSNativeObjectValuePropertyCallbacks that describe the
	  JSNativeObject's value properties.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_value_property_callbacks(const std::unordered_set<JSNativeObjectValuePropertyCallback<T>>& value_property_callbacks) {
		value_property_callbacks_ = value_property_callbacks;
		return *this;
	}
	
	/*!
	  @method

	  @abstract Return the current set of
	  JSNativeObjectFunctionPropertyCallbacks that describe the
	  JSNativeObject's function properties.

	  @result The current set of of
	  JSNativeObjectFunctionPropertyCallbacks that describe the
	  JSNativeObject's function properties.
	*/
	std::unordered_set<JSNativeObjectFunctionPropertyCallback<T>> get_function_property_callbacks() const {
		return function_property_callbacks_;
	}
	
	/*!
	  @method

	  @abstract Replace the current set of of JSNativeObjectFunctionPropertyCallbacks that
	  describe the class's statically declared function properties.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_function_property_callbacks(const std::unordered_set<JSNativeObjectFunctionPropertyCallback<T>>& function_property_callbacks) {
		function_property_callbacks_ = function_property_callbacks;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when a JavaScript object
	  is first created.
	  
	  @result The callback to invoke when a JavaScript object is first
	  created.
	*/
	InitializeCallback<T> get_initialize_callback() const {
		return initialize_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  first created. Use this callback to perform any additional
	  initialization in your native C++ object that wasn't done in its
	  constructor.
	  
	  @discussion It is recommended that you perform your object's
	  initialization in your C++ class constructor instead of using the
	  InitializeCallback. The InitializeCallback is of limited use and
	  is provided only for parity with the JavaScriptCore C API.

	  Unlike the other object callbacks, the initialize callback is
	  called on the least derived object (the parent object) first, and
	  the most derived object last, analogous to that way C++
	  constructors work in a class hierarchy.

	  For example, given this class definition:

	  class Foo {
	    void Initialize();
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_initialize_callback(&Foo::Initialize);
  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_initialize_callback(const InitializeCallback<T>& initialize_callback) {
		initialize_callback_ = initialize_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when a JavaScript object
	  is finalized (prepared for garbage collection).
  	  
	  @result The callback to invoke when a JavaScript object is
	  finalized (prepared for garbage collection).
	*/
	FinalizeCallback<T> get_finalize_callback() const {
		return finalize_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  finalized (prepared for garbage collection). This callback is
	  invoked immediately before your C++ class destructor. An object
	  may be finalized on any thread.

	  @discussion It is recommended that you release your object's
	  resources and perform other cleanup in your object's destructor
	  instead of using this callback. The FinalizeCallback is of limited
	  use and is provided only for parity with the JavaScriptCore C API.
	  
	  The finalize callback is called on the most derived object first,
	  and the least derived object (the parent object) last, analogous
	  to that way C++ destructors work in a class hierarchy.
	  
	  You must not call any function that may cause a garbage collection
	  or an allocation of a garbage collected object from within a
	  FinalizeCallback. This basically means don't create any object
	  whose class name begins with JS (e.g. JSString, JSValue, JSObject,
	  etc.)  and don't call any methods on such objects that you may
	  already have a reference to.
	  
	  For example, given this class definition:

	  class Foo {
	    void Finalize();
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_finalize_callback(&Foo::Finalize);

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_finalize_callback(const FinalizeCallback<T>& finalize_callback) {
		finalize_callback_ = finalize_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when determining whether a
	  JavaScript object has a property.
  
	  @result The callback to invoke when determining whether a
	  JavaScript object has a property.
	*/
	HasPropertyCallback<T> get_has_property_callback() const {
		return has_property_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when determining whether a
	  JavaScript object has a property. If this callback is missing then
	  the object will delegate to GetPropertyCallback.
  
	  @discussion The HasPropertyCallback enables optimization in cases
	  where only a property's existence needs to be known, not its
	  value, and computing its value is expensive. If the
	  HasPropertyCallback doesn't exist, then the GetPropertyCallback
	  will be used instead.

	  If this function returns false, the hasProperty request forwards
	  to the native object's JSNativeObjectPropertyCallback (if any),
	  then its parent JSNativeObject chain (which includes the default
	  object class), then its prototype chain.

	  For example, given this class definition:

	  class Foo {
	    bool HasProperty(const JSString& property_name) const;
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_has_property_callback(&Foo::HasProperty);

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_has_property_callback(const HasPropertyCallback<T>& has_property_callback) {
		has_property_callback_ = has_property_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when getting a property's
	  value from a JavaScript object.
  	  
	  @result The callback to invoke when getting a property's value
	  from a JavaScript object.
	*/
	GetPropertyCallback<T> get_get_property_callback() const {
		return get_property_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when getting a property's
	  value from a JavaScript object.
	  
	  @discussion If this function returns JSUndefined, the get request
	  forwards to the native object's JSNativeObjectPropertyCallback (if
	  any), then its parent JSNativeObject chain (which includes the
	  default object class), then its prototype chain.

	  For example, given this class definition:

	  class Foo {
	    JSValue GetProperty(const JSString& property_name) const;
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_get_property_callback(&Foo::GetProperty);
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_get_property_callback(const GetPropertyCallback<T>& get_property_callback) {
		get_property_callback_ = get_property_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when setting a property's
	  value on a JavaScript object.
  	  
	  @result The callback to invoke when setting a property's value on
	  a JavaScript object.
	*/
	SetPropertyCallback<T> get_set_property_callback() const {
		return set_property_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when setting a property's
	  value on a JavaScript object.

	  @discussion If this function returns false, the set request
	  forwards to the native object's JSNativeObjectPropertyCallback (if
	  any), then its parent JSNativeObject chain (which includes the
	  default object class), then its prototype chain.
  
	  For example, given this class definition:

	  class Foo {
	    bool SetProperty(const JSString& property_name, const JSValue& value);
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_set_property_callback(&Foo::SetProperty);
  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_set_property_callback(const JSObject::SetPropertyCallback& set_property_callback) {
		set_property_callback_ = set_property_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when deleting a property
	  from a JavaScript object.
  	  
	  @result The callback to invoke when deleting a property from a
	  JavaScript object.
	*/
	DeletePropertyCallback<T> get_delete_property_callback() const {
		return delete_property_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when deleting a property from
	  a JavaScript object.
  	  
	  @discussion If this function returns false, the delete request
	  forwards to the native object's JSNativeObjectPropertyCallback (if
	  any), then its parent JSNativeObject chain (which includes the
	  default object class), then its prototype chain.

	  For example, given this class definition:

	  class Foo {
	    bool DeleteProperty(const JSString& property_name);
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_delete_property_callback(&Foo::DeleteProperty);

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_delete_property_callback(const DeletePropertyCallback<T>& delete_property_callback) {
		delete_property_callback_ = delete_property_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when collecting the names
	  of a JavaScript object's properties.
  	  
	  @result The callback to invoke when collecting the names of a
	  JavaScript object's properties
	*/
	GetPropertyNamesCallback<T> get_get_property_names_callback() const {
		return get_property_names_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when collecting the names of
	  a JavaScript object's properties.
	  
	  @discussion The GetPropertyNamesCallback only needs to provide the
	  names of properties that the native object manages through
	  GetPropertyNamesCallback or SetPropertyCallback. Other properties,
	  including the native object's set of
	  JSNativeObjectPropertyCallback (if any), properties vended by
	  other objects, and properties belonging to object's prototype, are
	  added independently.

	  For example, given this class definition:

	  class Foo {
	    void GetPropertyNames(const JSPropertyNameAccumulator& accumulator) const;
	  };

	  You would call the builer like this:

	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_get_property_names_callback(&Foo::GetPropertyNames);
	  
	  Property name accumulators are used by JavaScript for...in loops.
	  Use JSPropertyNameAccumulator::AddName to add property names to
	  accumulator.

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_get_property_names_callback(const GetPropertyNamesCallback<T>& get_property_names_callback) {
		get_property_names_callback_ = get_property_names_callback;
		return *this;
	}

	/*!
	  @method

	  @abstract Return the callback to invoke when a JavaScript object
	  is called as a function.
  
	  @result The callback to invoke when a JavaScript object is called
	  as a function.
	*/
	CallAsFunctionCallback<T> get_call_as_function_callback() const {
		return call_as_function_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  called as a function.

	  @discussion If this callback does not exist, then calling your
	  object as a function will throw an exception.

	  For example, given this class definition:
	  
	  class Foo {
	    JSValue CallAsFunction(const std::vector<JSValue>& arguments);
	  };
	  
	  You would call the builer like this:
	  
	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_call_as_function_callback(&Foo::CallAsFunction);
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_call_as_function_callback(const CallAsFunctionCallback<T>& call_as_function_callback) {
		call_as_function_callback_ = call_as_function_callback;
		return *this;
	}

	/*!
	  @method
	  
	  @abstract Return the callback to invoke when a JavaScript object
	  is called as a function when it is a property of another
	  JavaScript object.
  
	  @result The callback to invoke when a JavaScript object is called
	  as a function when it is a property of another JavaScript object.
	*/
	CallAsFunctionWithThisCallback<T> get_call_as_function_with_this_callback() const {
		return call_as_function_with_this_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  called as a function when it is a property of another JavaScript
	  object.

	  @discussion If this callback does not exist, then calling your
	  object as a function when it is a property of another JavaScript
	  object will throw an exception.
	  
	  If this callback does not exist, then calling your object as a
	  function when it is a property of another object will throw an
	  exception.

	  For example, given this class definition:
	  
	  class Foo {
	    JSValue CallAsFunction(const std::vector<JSValue>& arguments, const JSObject& this_object);
	  };
	  
	  You would call the builer like this:
	  
	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_call_as_function_with_this_callback(&Foo::CallAsFunction);
	  
	  In the JavaScript expression 'myObject.myFunction()', then
	  'myFunction' is the instance of Foo being called, and this_object
	  would be set to 'myObject'.

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_call_as_function_with_this_callback(const CallAsFunctionWithThisCallback<T>& call_as_function_with_this_callback) {
		call_as_function_with_this_callback_ = call_as_function_with_this_callback;
		return *this;
	}
	
	/*!
	  @method
	  
	  @abstract Return the callback to invoke when a JavaScript object
	  is used as a constructor in a 'new' expression. If you provide
	  this callback then you must also provide the HasInstanceCallback
	  as well.
  
	  @result The callback to invoke when an object is used as a
	  constructor in a 'new' expression.
	*/
	CallAsConstructorCallback<T> get_call_as_constructor_callback() const {
		return call_as_constructor_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  used as a constructor in a 'new' expression. If you provide this
	  callback then you must also provide the HasInstanceCallback as
	  well.

	  @discussion If this callback doest not exist, then using your
	  object as a constructor in a 'new' expression will throw an
	  exception.
	  
	  For example, given this class definition:
	  
	  class Foo {
	    JSObject CallAsConstructor(const std::vector<JSValue>& arguments);
	  };
	  
	  You would call the builer like this:
	  
	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_call_as_constructor_callback(&Foo::CallAsConstructor);

	  If your callback were invoked by the JavaScript expression
	  'new myConstructor()', then 'myConstructor' is the instance of Foo
	  being called.

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_call_as_constructor_callback(const CallAsConstructorCallback<T>& call_as_constructor_callback) {
		call_as_constructor_callback_ = call_as_constructor_callback;
		return *this;
	}
	
	/*!
	  @method
	  
	  @abstract Return the callback to invoke when a JavaScript object
	  is used as the target of an 'instanceof' expression. If you
	  provide this callback then you must also provide the
	  CallAsConstructorCallback as well.

	  @result The callback to invoke when an object is used as the
	  target of an 'instanceof' expression.
	*/
	HasInstanceCallback<T> get_has_instance_callback() const {
		return has_instance_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when a JavaScript object is
	  used as the target of an 'instanceof' expression. If you provide
	  this callback then you must also provide the
	  CallAsConstructorCallback as well.

	  @discussion If this callback does not exist, then 'instanceof'
	  expressions that target your object will return false.

	  For example, given this class definition:
	  
	  class Foo {
	    bool HasInstance(const JSValue& possible_instance) const;
	  };

	  You would call the builer like this:
	  
	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_has_property_callback(&Foo::HasInstance);

	  If your callback were invoked by the JavaScript expression
	  'someValue instanceof myObject', then 'myObject' is the instanceof
	  of Foo being called and 'someValue' is the possible_instance
	  parameter.
	  
	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_has_instance_callback(const HasInstanceCallback<T>& has_instance_callback) {
		has_instance_callback_ = has_instance_callback;
		return *this;
	}
	
	/*!
	  @method

	  @abstract Return the callback to invoke when converting a
	  JavaScript object another JavaScript type.

	  @result The callback to invoke when converting a JavaScript object
	  to another JavaScript type.
	*/
	ConvertToTypeCallback<T> get_convert_to_type_callback() const {
		return convert_to_type_callback_;
	}

	/*!
	  @method

	  @abstract Set the callback to invoke when converting a JavaScript
	  object another JavaScript type.

	  @discussion If this function returns JSUndefined, the conversion
	  request forwards to JSNativeObject's parent chain (which includes
	  the default object class).
  
	  This function is only invoked when converting an object to number
	  or string. An object converted to boolean is 'true.' An object
	  converted to object is itself.

	  For example, given this class definition:
	  
	  class Foo {
	    JSValue ConvertToType(const JSValue::Type& type) const;
	  };

	  You would call the builer like this:
	  
	  JSNativeObjectBuilder<Foo> builder;
	  builder.set_convert_to_type_callback(&Foo::ConvertToType);

	  @result A reference to the builder for chaining.
	*/
	JSNativeObjectBuilder& set_convert_to_type_callback(const ConvertToTypeCallback<T>& convert_to_type_callback) {
		convert_to_type_callback_ = convert_to_type_callback;
		return *this;
	}
	
	/*!
	  @method

	  @abstract Create and return a JSClass instance with all of the
	  properties and callbacks specified in the builder.

	  @result A JSClass instance with all of the properties and
	  callbacks specified in the builder.
	*/
	JSClass2 build() const {
		// TODO validate js_class_definition.
		return JSClass2(*this);
	}
	
 private:

	JSNativeObjectDefinition<T> get_js_native_object_definition() const {
		JSNativeObjectDefinition<T> js_native_object_definition(*this);
		return js_native_object_definition;
	}

	// Require parameters
	JSContext js_context_;
	
	// Optional parameters - initialized to default values
	JSString                                                   class_name_;
	std::unordered_set<JSNativeObjectAttributes>               attributes_;
	std::shared_ptr<JSNativeObject>                            parent_ptr_                          { nullptr };
	std::unordered_set<JSNativeObjectValuePropertyCallback>    value_property_callbacks_;
	std::unordered_set<JSNativeObjectFunctionPropertyCallback> function_property_callbacks_;
	InitializeCallback                                         initialize_callback_                 { nullptr };
	FinalizeCallback<T>                                        finalize_callback_                   { nullptr };
	HasPropertyCallback<T>                                     has_property_callback_               { nullptr };
	GetPropertyCallback<T>                                     get_property_callback_               { nullptr };
	SetPropertyCallback<T>                                     set_property_callback_               { nullptr };
	DeletePropertyCallback<T>                                  delete_property_callback_            { nullptr };
	GetPropertyNamesCallback<T>                                get_property_names_callback_         { nullptr };
	CallAsFunctionCallback<T>                                  call_as_function_callback_           { nullptr };
	CallAsConstructorCallback<T>                               call_as_constructor_callback_        { nullptr };
	CallAsFunctionWithThisCallback<T>                          call_as_function_with_this_callback_ { nullptr };
	HasInstanceCallback<T>                                     has_instance_callback_               { nullptr };
	ConvertToTypeCallback<T>                                   convert_to_type_callback_            { nullptr };
};

template<typename T>
JSNativeObjectDefinition<T>::JSNativeObjectDefinition(const JSNativeObjectBuilder<T>& builder)
		: class_name_(builder.class_name_)
		, class_name_for_js_class_definition_(class_name_)
		, attributes_(builder.attributes_)
		, parent_ptr_(builder.parent_ptr_)
		, value_property_callbacks_(builder.value_property_callbacks_)
		, function_property_callbacks_(builder.function_property_callbacks_)
		, initialize_callback_(builder.initialize_callback_)
		, finalize_callback_(builder.finalize_callback_)
		, has_property_callback_(builder.has_property_callback_)
		, get_property_callback_(builder.get_property_callback_)
		, set_property_callback_(builder.set_property_callback_)
		, delete_property_callback_(builder.delete_property_callback_)
		, get_property_names_callback_(builder.get_property_names_callback_)
		, call_as_function_with_this_callback_(builder.call_as_function_with_this_callback_)
		, call_as_function_callback_(builder.call_as_function_callback_)
		, call_as_constructor_callback_(builder.call_as_constructor_callback_)
		, has_instance_callback_(builder.has_instance_callback_)
		, convert_to_type_callback_(builder.convert_to_type_callback_) {
}

template<typename T>
JSNativeObject<T>::JSNativeObject(const JSNativeObjectBuilder<T>& builder)
		: js_context_(builder.js_context_)
		, js_native_object_definition_(builder.get_js_native_object_definition()) {

	// Use kJSClassAttributeNoAutomaticPrototype in combination with JSObjectSetPrototype to manage prototypes manually
	
	using native_object_attribute_underlying_type = std::underlying_type<JSNativeObjectAttributes>::type;
	std::bitset<2> native_object_attributes;
	for (auto native_object_attribute : js_native_object_definition_.attributes_) {
		const auto bit_position = static_cast<native_object_attribute_underlying_type>(native_object_attribute);
		native_object_attributes.set(bit_position);
	}

	js_class_definition_.attributes = static_cast<native_object_attribute_underlying_type>(native_object_attributes.to_ulong());
	js_class_definition_.className  = js_native_object_definition_.class_name_for_js_class_definition_.c_str();

	if (js_native_object_definition_.parent_ptr_) {
		js_class_definition_.parentClass = js_native_object_definition_.parent_ptr_ -> js_class_;
	}
	
	// TODO
	for (const auto& value_property_callback : js_native_object_definition_.value_property_callbacks_) {
	}

	if (js_native_object_definition_.initialize_callback_) {
		js_class_definition_.initialize = JSObjectInitializeCallback;
	}

	js_class_ = JSClass(&js_class_definition_);
}

}} // namespace JavaScriptCoreCPP { namespace RAII {

#endif // _JAVASCRIPTCORECPP_RAII_JSNATIVEOBJECTBUILDER_HPP_