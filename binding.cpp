#include <iostream>
#include <assert.h>

#include <bare.h>
#include <js.h>

#define DEBUG

#ifdef DEBUG
  #define CHECKPT std::cout << __func__ << ":" << __LINE__ << '\n';
#endif

/**
 * Comment out line 66 and uncomment line 67. The program again crashes silently.
 * Initializing data members via constructor is an issue?
*/

class Test {
public:
    std::string m_name;

    /**
     * PROBLEM: Initializing an object with constructor causes issues
    */
    Test(char* name) { m_name = name; }

    void accessName() const
    {
        std::cout << "Inside: " << __func__ << "!\n";

        /**
         * OK: Accessing data member works
         */
        std::cout << "Name is: " << m_name << "\n";
    }
};


js_value_t* jsCreateTestObjectCb(js_env_t* env,
                                 js_callback_info_t* info)
{
    size_t jsArgc = 0;
    js_value_t** jsArgv = nullptr;
    js_value_t* jsObject = nullptr;
    size_t len = 0;
    utf8_t* str = nullptr;

    Test* test = nullptr;

    {
        js_get_callback_info(env, info, &jsArgc, nullptr, nullptr, nullptr);
        assert(jsArgc == 1);

        jsArgv = new js_value_t*[jsArgc];
        js_get_callback_info(env, info, &jsArgc, jsArgv, nullptr, nullptr);
    }

    js_get_value_string_utf8(env, jsArgv[0], nullptr, 0, &len);
    str = new utf8_t[len + 1];
    str[len] = '\0';

    js_get_value_string_utf8(env, jsArgv[0], str, len, nullptr);

    js_create_arraybuffer(env, sizeof(Test), reinterpret_cast<void**>(&test), &jsObject);
    assert(jsObject);

    test->m_name = reinterpret_cast<char*>(str);
    // test = new Test(reinterpret_cast<char*>(str));

    return jsObject;
}


js_value_t* jsAccessNameOfObjectCb(js_env_t* env,
                                   js_callback_info_t* info)
{
    size_t jsArgc = 0;
    js_value_t** jsArgv = nullptr;
    js_value_t* result = nullptr;
    size_t bytes = 0;
    Test* test = nullptr;

    js_get_callback_info(env, info, &jsArgc, nullptr, nullptr, nullptr);
    assert(jsArgc == 1);

    jsArgv = new js_value_t*[jsArgc];
    js_get_callback_info(env, info, &jsArgc, jsArgv, nullptr, nullptr);

    js_get_arraybuffer_info(env, jsArgv[0], reinterpret_cast<void**>(&test), &bytes);

    /**
     * OK: Works
    */
    test->accessName();

    js_get_undefined(env, &result);

    return result;
}


static js_value_t *
init(js_env_t* env, js_value_t* exports)
{
    js_value_t* createTestObjectFunc;
    js_value_t* accessNameOfObjectFunc;

    {
        js_create_function(env, "createTestObject", -1, reinterpret_cast<js_function_cb>(jsCreateTestObjectCb), NULL, &createTestObjectFunc);
        js_set_named_property(env, exports, "createTestObject", createTestObjectFunc);
    }

    {
        js_create_function(env, "accessNameOfObject", -1, reinterpret_cast<js_function_cb>(jsAccessNameOfObjectCb), NULL, &accessNameOfObjectFunc);
        js_set_named_property(env, exports, "accessNameOfObject", accessNameOfObjectFunc);
    }

    return exports;
}


BARE_MODULE(first, init)
