const binding = require("./binding");

/* Create a C++ object with passed string */
const testObject = binding.createTestObject("Foobar");


binding.accessNameOfObject(testObject);