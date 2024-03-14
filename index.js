const binding = require("./binding");

/* Create a C++ object with passed string */
const testObject = binding.createTestObject("Foobarbaz");
binding.accessNameOfObject(testObject);