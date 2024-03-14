#include <iostream>
#include <assert.h>

#include <bare.h>
#include <js.h>

class Test {
    std::string m_name;

public:
    Test(char* name)
    {
        m_name = name;
    }

    void accessName()
    {
        /**
         * OK: We don't access any data member
         */
        std::cout << "Inside: " << __func__ << "!\n";

        /**
         * ERROR: m_name strangely no longer accessible
         */
        std::cout << "Name is: " << m_name << "\n\n";
    }
};

void jsCreateTestObjectCb(js_env_t* env,
                          js_callback_info_t* info,
                          js_value_t** jsResult)
{
    size_t jsArgc = 0;
    js_value_t** jsArgv = nullptr;
    js_value_t* jsObject = nullptr;
    Test* test = nullptr;

    /* Collect the passed argument passed from JS */
    js_get_callback_info(env, info, &jsArgc, nullptr, nullptr, nullptr);
    jsArgv = new js_value_t*[jsArgc];
    js_get_callback_info(env, info, &jsArgc, jsArgv, nullptr, nullptr);

    /* This block extracts the passed string argument and creates a Test object from it */
    {
        size_t len = 0;
        utf8_t* str = nullptr;

        /* First call to js_get_value_string_utf8: Get the length of string argument */
        js_get_value_string_utf8(env, jsArgv[0], nullptr, 0, &len);
        str = new utf8_t[len + 1];
        str[len] = '\0';

        /* Second call: Fill variable `str` with the value */
        js_get_value_string_utf8(env, jsArgv[0], str, len, nullptr);

        /* Create a Test object with extracted string argument */
        test = new Test(reinterpret_cast<char*>(str));
    }

    /**
     * OK: Works fine for this function call i.e., createTestObject() [index.js:3]
    */
    test->accessName();

    /* Make the Test object available in JS as well */
    js_create_external(
        env,
        test,
        [] (js_env_t* env, void* data, void* hint) {
            delete static_cast<Test*>(data);
        },
        nullptr,
        &jsObject
    );

    jsResult = &jsObject;
}


void jsAccessNameOfObjectCb(js_env_t* env,
                            js_callback_info_t* info,
                            js_value_t** jsResult)
{
    size_t jsArgc = 0;
    js_value_t** jsArgv = nullptr;
    js_value_t* result = nullptr;
    void* externalData = nullptr;
    Test* test = nullptr;

    /* Collect the passed argument passed from JS */
    js_get_callback_info(env, info, &jsArgc, nullptr, nullptr, nullptr);
    jsArgv = new js_value_t*[jsArgc];
    js_get_callback_info(env, info, &jsArgc, jsArgv, nullptr, nullptr);

    {
        /* Get the external object passed from JS */
        js_get_value_external(env, jsArgv[0], &externalData);
        assert(externalData);

        test = static_cast<Test*>(externalData);
    }

    /**
     * PROBLEM: The member function itself is accessible, but not the
     * data members of the class object.
    */
    test->accessName();

    js_get_undefined(env, &result);
    jsResult = &result;
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
