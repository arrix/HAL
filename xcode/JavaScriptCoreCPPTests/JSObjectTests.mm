/**
 * JavaScriptCoreCPP
 * Author: Matthew D. Langston
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "JavaScriptCoreCPP/JavaScriptCoreCPP.hpp"
#include <unordered_set>
#include <limits>
#include <cstdint>
#import <XCTest/XCTest.h>

using namespace JavaScriptCoreCPP;

namespace UnitTestConstants {
  static const double pi { 3.141592653589793 };
}

@interface JSObjectTests : XCTestCase
@end

@implementation JSObjectTests {
  JSContextGroup js_context_group;
}

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testObjectSizes {
  XCTAssertEqual(sizeof(std::intptr_t)                         , sizeof(JSContextGroup));
  XCTAssertEqual(sizeof(std::intptr_t) + sizeof(JSContextGroup), sizeof(JSContext));
  
  // JSValue and JSObject are base classes, so have an extra pointer for the
  // virtual function table.
  XCTAssertEqual(sizeof(std::intptr_t) + sizeof(std::intptr_t) + sizeof(JSContext), sizeof(JSValue));
  XCTAssertEqual(sizeof(std::intptr_t) + sizeof(std::intptr_t) + sizeof(JSContext), sizeof(JSObject));
}

- (void)testJSPropertyAttribute {
  std::unordered_set<JSPropertyAttribute> attributes;
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::None));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::ReadOnly));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::DontEnum));
  XCTAssertEqual(0, attributes.count(JSPropertyAttribute::DontDelete));
  
  const auto insert_result = attributes.insert(JSPropertyAttribute::DontDelete);
  XCTAssertTrue(insert_result.second);
  XCTAssertEqual(1, attributes.count(JSPropertyAttribute::DontDelete));
  XCTAssertEqual(*insert_result.first, JSPropertyAttribute::DontDelete);

  XCTAssertEqual(1, attributes.size());
}

- (void)testJSObject {
  JSContext js_context = js_context_group.CreateContext();
  JSObject js_object = js_context.CreateObject();
  JSValue prototype = js_object.GetPrototype();
  XCTAssertFalse(prototype.IsUndefined());
  XCTAssertFalse(prototype.IsNull());
  XCTAssertFalse(prototype.IsBoolean());
  XCTAssertFalse(prototype.IsNumber());
  XCTAssertFalse(prototype.IsString());
  XCTAssertTrue(prototype.IsObject());

  XCTAssertFalse(js_object.HasProperty("foo"));
  XCTAssertTrue(js_object.GetProperty("foo").IsUndefined());
  
  js_object.SetProperty("foo", js_context.CreateNumber(42));
  XCTAssertTrue(js_object.HasProperty("foo"));
  JSNumber foo = js_object.GetProperty("foo");
  XCTAssertEqual(42, static_cast<int32_t>(42));
  XCTAssertTrue(js_object.DeleteProperty("foo"));

  js_object.SetProperty("bar", js_context.CreateBoolean(true), {JSPropertyAttribute::DontDelete});
  XCTAssertTrue(js_object.HasProperty("bar"));
  JSBoolean bar = js_object.GetProperty("bar");
  XCTAssertTrue(static_cast<bool>(bar));
  XCTAssertFalse(js_object.DeleteProperty("bar"));
  XCTAssertTrue(js_object.HasProperty("bar"));
  bar = js_object.GetProperty("bar");
  XCTAssertTrue(static_cast<bool>(bar));

  XCTAssertTrue(js_object.GetProperty(42).IsUndefined());
  js_object.SetProperty(42, js_context.CreateNumber(UnitTestConstants::pi));
  JSNumber pi = js_object.GetProperty(42);
  XCTAssertEqualWithAccuracy(UnitTestConstants::pi, static_cast<double>(pi), std::numeric_limits<double>::epsilon());

  XCTAssertFalse(js_object.IsFunction());
  
  try {
    js_object();
    XCTFail("js_object was called as a function but did not throw a std::runtime_error exception");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }

  XCTAssertFalse(js_object.IsConstructor());
  
  try {
    js_object.CallAsConstructor();
    XCTFail("js_object was called as a constructor but did not throw a std::runtime_error exception");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }

  auto js_value  = js_context.JSEvaluateScript("new Object()");
  XCTAssertTrue(js_value.IsObject());

  js_value  = js_context.JSEvaluateScript("Object()");
  XCTAssertTrue(js_value.IsObject());
  
  // It is surprising to me that an object literal, "{}", is not an object.
  js_value  = js_context.JSEvaluateScript("{}");
  XCTAssertTrue(js_value.IsUndefined());

  // But this is an object.
  js_value  = js_context.JSEvaluateScript("var a = {}; a");
  XCTAssertTrue(js_value.IsObject());
  
  // This is nor a primitive string.
  js_value  = js_context.JSEvaluateScript("new String()");
  XCTAssertTrue(js_value.IsObject());
  XCTAssertFalse(js_value.IsString());

  // Yet this is a primitive string (i.e. without new).
  js_value  = js_context.JSEvaluateScript("String()");
  XCTAssertFalse(js_value.IsObject());
  XCTAssertTrue(js_value.IsString());

  js_value  = js_context.JSEvaluateScript("new Date()");
  XCTAssertTrue(js_value.IsObject());

  js_value  = js_context.JSEvaluateScript("new Array()");
  XCTAssertTrue(js_value.IsObject());

  // An array literal is an Object, as expected. Why isn't an object literal,
  // "{}", an Object?
  js_value  = js_context.JSEvaluateScript("[]");
  XCTAssertTrue(js_value.IsObject());

  js_value  = js_context.JSEvaluateScript("[1, 3, 5, 7]");
  XCTAssertTrue(js_value.IsObject());
}

- (void)testProperty {
  JSContext js_context = js_context_group.CreateContext();
  auto js_value  = js_context.JSEvaluateScript("[1, 3, 5, 7]");
  XCTAssertTrue(js_value.IsObject());
  JSObject js_object = js_value;
  XCTAssertTrue(js_object.HasProperty("length"));
  js_value = js_object.GetProperty("length");
  XCTAssertEqual(4, static_cast<int32_t>(js_value));

  js_value  = js_context.JSEvaluateScript("'hello, JavaScript'");
  XCTAssertTrue(js_value.IsString());
  js_object = js_value;
  XCTAssertTrue(js_object.HasProperty("length"));
  js_value = js_object.GetProperty("length");
  XCTAssertEqual(17, static_cast<int32_t>(js_value));
  
  // Trying to reference an undefined property returns undefined.
  XCTAssertFalse(js_object.HasProperty("foo"));
  js_value = js_object.GetProperty("foo");
  XCTAssertTrue(js_value.IsUndefined());

  js_value = js_context.JSEvaluateScript("new Object()");
  XCTAssertTrue(js_value.IsObject());
  js_object = js_value;
  XCTAssertFalse(js_object.HasProperty("foo"));
  
  js_object.SetProperty("foo", js_context.CreateNumber(42));
  XCTAssertTrue(js_object.HasProperty("foo"));
  js_value = js_object.GetProperty("foo");
  XCTAssertEqual(42, static_cast<int32_t>(js_value));

  // Should be able to delete a property we created.
  XCTAssertTrue(js_object.DeleteProperty("foo"));

  // It is legal to delete a non-existent property.
  XCTAssertTrue(js_object.DeleteProperty("bar"));
  
  const std::string quoteString = "Life can only be understood backwards; but it must be lived forwards.";
  
  // You can set a custom propery on a string.
  js_value = js_context.CreateString("hello, world");
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on a bool.
  js_value = js_context.CreateBoolean(true);
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on a number.
  js_value = js_context.CreateNumber(42);
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an array.
  js_value = js_context.CreateArray();
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an regexp.
  js_value = js_context.CreateRegExp();
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can set a custom propery on an error.
  js_value = js_context.CreateError();
  js_object = js_value;
  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));

  // You can't set a custom propery on a undefined.
  js_value = js_context.CreateUndefined();
  try {
    js_object = js_value;
    XCTFail("js_value illegally converted to a JSObject");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }

  // You can't set a custom propery on a null.
  js_value = js_context.CreateNull();
  try {
    js_object = js_value;
    XCTFail("js_value illegally converted to a JSObject");
  } catch (const std::runtime_error& exception) {
    XCTAssert(YES, @"Caught expected std::runtime_error exception.");
  } catch (...) {
    XCTFail("Caught unexpected unknown exception, but we expected a std::runtime_error exception.");
  }

  js_object.SetProperty("quote", js_context.CreateString(quoteString));
  XCTAssertTrue(js_object.HasProperty("quote"));
  js_value = js_object.GetProperty("quote");
  XCTAssertEqual(quoteString, static_cast<std::string>(js_value));
}

- (void)testJSArray {
  JSContext js_context = js_context_group.CreateContext();
  JSArray js_array = js_context.CreateArray();
}

- (void)testJSDate {
  JSContext js_context = js_context_group.CreateContext();
  JSDate js_date = js_context.CreateDate();
}

- (void)testJSError {
  JSContext js_context = js_context_group.CreateContext();
  JSError js_error = js_context.CreateError();
}

- (void)testJSRegExp {
  JSContext js_context = js_context_group.CreateContext();
  JSRegExp js_regexp = js_context.CreateRegExp();
}

- (void)testJSFunction {
  JSContext js_context = js_context_group.CreateContext();
  JSFunction js_function = js_context.CreateFunction("return 'Hello, ' + name;", {"name"});
  XCTAssertTrue(js_function.IsFunction());
  //std::clog << "MDL: js_function(\"world\") = " << js_function("world") << std::endl;
  XCTAssertEqual("Hello, world", static_cast<std::string>(js_function("world")));
}

// As of 2014.09.20 Travis CI only supports Xcode 5.1 which lacks support for
// measureBlock.
#ifndef TRAVIS
- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}
#endif

@end
