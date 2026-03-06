
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#pragma once

#ifdef TEST_ENABLED

// set to true if you want to break on all test failures immediately
#define __TEST_INFRA_BREAK_ON_FAIL true

// by default msvc will execute static initialization from top to bottom on file
// causing the file to execute from bottom up when the list read.
// Some compilers do this in reverse from msvc. Set to true to manually reverse list
// after created.
#define __TEST_INFRA_REVERSE_FN_ORDER true

/*
TestInfra
This header is a light test infrasturcture system that requires no registration of test functions or classes, just use the macros to create the tests and run them.

Example:
```
#include <util/TestInfra.hpp>

TEST_CLASS(SomeTestClassName_MustBeUniqueToAllOtherTestClasses) {
    // whatever members you wish
    // whatever member functions you wish

    TEST(TestNameHere_MustBeUniqueInClass) {
        // Arrange
        // Act
        // Assert
        EXPECT_TRUE(true, "some message for failures");
    }

    TEST(AnotherTest) {
        EXPECT_FAIL("This test always fails");
    }

    TEST(SomeTest, "TagForTest") {} // tags will allow you to run by name or the tag
    // tags can be shared between any test or class
};

TEST_CLASS(Foo, "SomeTag") {};

// tests or classes marked with the ignore flag will not be run unless explicitly named
TEST_CLASS(Foo2, TestInfra::Tags::Ignore) {};

int main() {
    // wont run Foo2
    TestInfra::TestRunner::runAllTests();

    // only runs SomeTestClassName_MustBeUniqueToAllOtherTestClasses::SomeTest
    TestInfra::TestRunner::runTests({ "TagForTest" });
}

```

API:
    -Test Tags:
        - Test tags are used to combine multiple tests or classes into a group of tests that should be run together
        - For example, if you have a series of Physics tests and more specifically a series of Collision tests
          then you can tag the Phsyics tests with "Physics" and the Collision tests with "Physics" and "Collision".
          Now you can use "Physics" to run all physics related tasks and "Collision" to only run the subset.
        - The test class name is always the first tag of a class
        - The test name is always the first tag of a test
        - If a test class is run, then all non-ignored tests will be run in the class
        - Special Tags:
            - Tags::Ignore: The ignore tag is used to prevent broken or incomplete tests from being run unless
                            specifically stated by using the test or class name
    - TEST_CLASS(_className, ...)
        - Arguments:
            - _className: Not a string, the name of your class
            - ...: Remaining arguments should strings which are the tags of the test class
        - Creates and auto registers your test class to be run by the system
        - The system requires classes, if you dont need a class then just make once and place your tests inside
        - Classes are fully instantiated before running the tests in the class
        - If you must know what test is about to run, use _getCurrFuncTestName() to get the name of the test about to run
          however, in general test start and stop functions are supposed to setup and shutdown common things used by all tests
        - Test classes allow you to create any additional members or member functions you want, however the constructor MUST
          be a default constructor. These members and member functions can be helpers and other common things used by tests
        - Any number of tests can be included in a test class
    - TEST_CLASS_INIT():
        - This function is optionally implemented for test classes. Will be called just after the class's constructor
    - TEST_CLASS_DROP():
        - This function is optionally implemented for test classes. Will be called just before the class's destructor
    - TEST_START():
        - This function is optionally implemented for test classes. Will be called before each test
    - TEST_STOP():
        - This function is optionally implemented for test classes. Will be called after each test has finished
    - TEST(_testName, ...):
        - Arguments:
            - _testName: Not a string, the name of your test function
            - ...: Remaining arguments should be strings which are the tags of the test function
        - Creates and auto registers your test function to the test class
        - The class this function is a member of will always be able to use other class members and member functions
    - TestRunner:
        - Runs tests. Before calling functions here, create your LogHook. If not created, the default CoutLogHook will be used
          which will simply output to stdout
        - runTests(aTags)
            - runs all tests that contain at least 1 of the tags in the tag list
            - will not run any tests with the ignore tag unless one of the tags is the test class name or test name
            - if no tags are provided, this calls runAllTests()
        - runAllTests()
            - runs all tests except those that have the ignore tag
        - If no tests are run, TestResult::None is returned as this could be an inconclusive result
    - TestResult
        - Enum value for the various results of a test
        - None: inconclusive result
        - Pass: Test passed
        - Fail: Test failed
    - LogHook
        - This class is used for logging inside a test. It is recommended that any logs you write as a part of a test
          should use the LogHook methods.
        - Calling from within a test:
            - error(aFormat, ...): Produces error, format is using `{}` to place arguments, arguments can be any type
            - log(aFormat, ...): Produces informational log, format is using `{}` to place arguments, arguments can be any type
        - Overloading for your own hook:
            - _testRunningInit(aTestCount): Called before running tests, provides the number of tests that will be run
            - _error(aMessage): Called when an error occurs in a test, provides the formatted message from the assert
            - _log(aMessage): Called when an information log happens in a test, provides the formatted message from the log
            - _startTest(aFuncName): Called before starting a test, provides the name of the test being run
            - _endTest(aFuncName, aResult): Called after running a test, provides the name of the test and the result
            - _startClass(aClassName): Called before creating the test class, before running any tests. Provides the class name
            - _endClass(aClassName): Called after destroying the test class, before running any tests. Provides the class name
    - Test Assertion Macros:
        - EXCPET_*: Except macros will not stop the test if they fail
        - ASSERT_*: Assert macros will stop the test and return if they fail
        - The following macros exist for both EXCEPT_* and ASSERT_*:
            - TRUE: The expression must be true
            - FALSE: The expression must be false
            - EQ: X must equal Y using operator==
            - NEQ: X must not equal Y using operator== and inverting the result
            - LTE: X must be less than or equal to Y using operator< and operator==
            - LT: X must be less than Y using operator<
            - GTE: X must be greater than Y using operator< and inverting the result
            - GT: X must be greater than Y using operator< (inverted) operator== (inverted)
            - NEAR: X must be near Y with an Epsilon (common with floating point numbers), abs(X - Y) < Epsilon
            - NOTNEAR: X must not be near Y with an Epsilon (common with floating point numbers), !(abs(X - Y) < Epsilon)
            - NULL: Expression must be NULL or nullptr
            - NOTNULL: Expression must not be NULL or nullptr
            - STR_NOCASE_EQ: String comparsion, X equals Y with no case checks using tolower on both and comparing
            - THROW: The action should throw and be caught by the catch expression. Produces error if not thrown or not caught by catch expression
            - ANYTHROW: The action should throw. Produces error if not thrown
            - NOTHROW: The action should not throw. Produces error if it does
*/

#include <format>   //  LogHook message formatting
#include <iostream> // CoutLogHook printing
#include <memory>   // unique_ptr used to create test class
#include <optional>
#include <string>
#include <vector>

#include <util/CompilerFlags.hpp>
#include <util/Benchmark.hpp>
#include <util/DebugBreak.hpp>
#include <util/Log.hpp>
#include <test/testHelpers/TestLogEndPoint.hpp>

namespace TestInfra {
    // forward declare all types
    struct Tags;
    enum class TestResult;
    class LogHook;
    namespace Internal {
        class CoutLogHook;
        struct ITestClassNode;
        struct TestInfraCollections;
        class TestTags;
        struct ITestClassNode;
        template <typename T>
        struct TestClassNode;
        struct ITestClass;
        template <typename T>
        struct TestClass;
        struct ITestFuncNode;
        template <typename tNodeClass, typename tClass>
        struct TestFuncNode;
        struct Assert;

        struct FailException : public std::exception {
            FailException() : std::exception() {};
        };
    }
    class TestRunner;

    // ===========================================================================================================================
    // The result of the test, None is inconclusive
    enum class TestResult { None, Pass, Fail };

    struct TestRunResult {
        TestResult mOveralResult = TestResult::None;
        size_t mTestsRun = 0;
        size_t mTestsFailed = 0;
        double mTimeToRun = 0.0; // in seconds
    };

    struct Tags {
        // Tests marked with this tag will not run unless explicitly named or the ignore tag is used
        static inline std::string Ignore = "__TEST_INFRA_IGNORE_TAG";
    };


    // ===========================================================================================================================
    // Implement from this to add your own loggers
    class LogHook {
        friend class TestRunner;
        static inline LogHook* sLogHook = nullptr;
        static LogHook& _get() {
            return *sLogHook;
        }

        // called when starting test run
        static void testRunnerInit(size_t aTestCount) {
            _get()._testRunnerInit(aTestCount);
        }

        // called before a test starts
        static void startTest(const std::string& aFuncName) {
            _get()._startTest(aFuncName);
        }

        // called after a test has finished
        static void endTest(const std::string& aFuncName, TestResult aResult, double timeMs) {
            _get()._endTest(aFuncName, aResult, timeMs);
        }

        // called before creating test class
        static void startClass(const std::string& aClassName) {
            _get()._startClass(aClassName);
        }

        // called after test class has been destroyed
        static void endClass(const std::string& aClassName, TestResult aResult, double timeMs) {
            _get()._endClass(aClassName, aResult, timeMs);
        }

    protected:
        virtual void _testRunnerInit(size_t /*aTestCount*/) {};
        virtual void _error(const std::string& /*aMessage*/) {};
        virtual void _log(const std::string& /*aMessage*/) {};
        virtual void _startTest(const std::string& /*aFuncName*/) {};
        virtual void _endTest(const std::string& /*aFuncName*/, TestResult /*aResult*/, double /*timeMs*/) {};
        virtual void _startClass(const std::string& /*aClassName*/) {};
        virtual void _endClass(const std::string& /*aClassName*/, TestResult /*aResult*/, double /*timeMs*/) {};

    public:
        // Only 1 hook can be used at a time
        LogHook() {
            sLogHook = this;
        }
        // cleans up hook in use
        virtual ~LogHook() {
            sLogHook = nullptr;
        }

        // returns true if a hook instance is available
        static bool hasHook() {
            return sLogHook;
        }

        // call to log an error or called when an assertion fails
        template <typename... tArgs>
        static void error(const std::string& aFormat, tArgs&&... aArgs) {
            _get()._error(std::vformat(aFormat, std::make_format_args(aArgs...)));
        }

        // call if you want to log anything
        template <typename... tArgs>
        static void log(const std::string& aFormat, tArgs&&... aArgs) {
            _get()._log(std::vformat(aFormat, std::make_format_args(aArgs...)));
        }
    };

    namespace Internal {
        // -------------------------------------------------------------------------------
        // Default log hook for when one is not provided
        // Will just print to stdout
        class CoutLogHook final : public LogHook {
        public:
            CoutLogHook() = default;
            ~CoutLogHook() override = default;
            void _error(const std::string& aMessage) override {
                std::cout << "TEST ERROR:\n" << aMessage << std::endl;
            }
            void _log(const std::string& aMessage) override {
                std::cout << "TEST LOG:\n" << aMessage << std::endl;
            }
            void _startTest(const std::string& aTestName) override {
                std::cout << "\t" << aTestName << std::endl;
            }
            void _endTest(const std::string&, TestResult aResult, double) override {
                std::string result = "Passed";
                if (aResult == TestResult::Fail) {
                    result = "Failed";
                }
                else if (aResult != TestResult::Pass) {
                    result = "Inconclusive, test did not finish";
                }
                std::cout << "\t\t" << result << std::endl;
            }
            void _startClass(const std::string& aTestClass) override {
                std::cout << aTestClass << std::endl;
            }
        };


        // ===========================================================================================================================
        // this is a static link list for test classes
        struct TestInfraCollections {
            static inline ITestClassNode* sTestClassListHead = nullptr;
        };


        // ===========================================================================================================================
        // Maintains the tags for a class or test
        class TestTags {
            std::vector<std::string> mTags;

            // Used to figure out if the first tag was specifically found as match or not
            enum class HasTagRet : char {
                Missing = 0,
                First = 1, // the first tag of classes or functions is always the name
                Other = 2,
            };

            // returns true if the tag exists in the list
            HasTagRet _hasTag(const std::string& aTag) const {
                bool first = false;
                for (auto&& tag : mTags) {
                    if (!first) {
                        first = true;
                        if (tag == aTag) {
                            return HasTagRet::First;
                        }
                        continue;
                    }
                    else if (tag == aTag) {
                        return HasTagRet::Other;
                    }
                }
                return HasTagRet::Missing;
            }

        public:
            TestTags(std::vector<std::string>&& aTags) : mTags(std::forward<std::vector<std::string>>(aTags)) {}

            // returns true if the tag was found
            bool hasTag(const std::string& aTag) const {
                return static_cast<char>(_hasTag(aTag)) > 0;
            }

            // returns true if the tag was found
            bool hasTag(const std::vector<std::string>& aTags) const {
                if (_hasTag(Tags::Ignore) == HasTagRet::Other) {
                    // since this has the ignored tag, we should only run it if we have a tag
                    // matching the name of this object which is always the first tag
                    for (auto&& tag : aTags) {
                        if (tag == mTags[0]) {
                            return true;
                        }
                    }
                    return false;
                }

                // not ignored, then if we find the tag, return true
                for (auto&& tag : aTags) {
                    if (static_cast<char>(_hasTag(tag)) > 0) {
                        return true;
                    }
                }
                return false;
            }
        };


        // ===========================================================================================================================
        // Interface to templated class to access members and store generically
        // Static link list node for test classes
        struct ITestClassNode {
            // creates an instance of the test class, unique_ptr is used as a base class is used to avoid templates
            virtual std::unique_ptr<ITestClass> createClass() const = 0;

            // returns the head of the function static link list
            virtual ITestFuncNode* getFuncList() = 0;

            // next pointer for static class node link list
            ITestClassNode* mNext = nullptr;
            std::string mClassName;
            TestTags mTags;

            // takes class name and any tags that this class uses
            ITestClassNode(const std::string& aName, std::vector<std::string>&& aTags)
                : mClassName(aName)
                , mTags(std::forward<std::vector<std::string>>(aTags)) {}

        };


        // -------------------------------------------------------------------------------
        // Templated version of ITestClassNode for specific implementations
        template <typename T>
        struct TestClassNode final : public ITestClassNode {
            // static link list for function nodes in this class
            static inline ITestFuncNode* sFuncNodes = nullptr;

            // takes class name and any tags that this class uses
            TestClassNode(const std::string& aName, std::vector<std::string>&& aTags)
                : ITestClassNode(aName, std::forward<std::vector<std::string>>(aTags)) {

                // handles the static link list update with this new node
                mNext = TestInfraCollections::sTestClassListHead;
                TestInfraCollections::sTestClassListHead = this;
            }

            // creates instance of the class and sets initial data
            std::unique_ptr<ITestClass> createClass() const override {
                std::unique_ptr<T> ptr = std::make_unique<T>();
                return std::move(ptr);
            }

            // returns the head of the function static link list
            ITestFuncNode* getFuncList() override {
                return sFuncNodes;
            }
        };


        // ===========================================================================================================================
        // Interface to templated class to access members and store generically
        // Mostly just used to call into specific functions that may need to be setup on the real test class instance
        struct ITestClass {
            friend class TestRunner; // sets the test name and class name

            // Shared between all test runs, reset to None at end of test
            static inline TestResult mTestResult = TestResult::None;
            // These are used for assertions
            static inline const std::string* mCurrFuncTestName = nullptr;
            static inline const std::string* mCurrClassName = nullptr;

            void onInit() { _onInit(); }
            void onDrop() { _onDrop(); }
            void onTestStart() { _onTestStart(); }
            void onTestStop() { _onTestStop(); }

        protected:
            // optionally implement these as the user
            virtual void _onInit() {}
            virtual void _onDrop() {}
            virtual void _onTestStart() {}
            virtual void _onTestStop() {}

        public:
            virtual ~ITestClass() = default;
        };


        // -------------------------------------------------------------------------------
        // Templated version of ITestClass for specific implementations
        template <typename T>
        struct TestClass : public ITestClass {
            // ClassType is used in Test macros to get the current class type
            using ClassType = T;
            using ClassNodeType = TestClassNode<T>;
        };


        // ===========================================================================================================================
        // helper for handling exceptions
        static void handleException(std::optional<std::exception*> e, ITestClass& testClass) {
            auto log = Log::testOnly_disableTestEndPoint();
            if (log) {
                log->printLogs();
            }
            if (e.has_value()) {
                if (__TEST_INFRA_BREAK_ON_FAIL) {
                    pdebugbreak();
                }
                Log::error(std::string("Test encountered exception: ") + e.value()->what());
            }
            testClass.mTestResult = TestResult::Fail;
        }


        // ===========================================================================================================================
        // Interface to templated class to access members and store generically
        // This node is used in the static link list of the test class this function is a member of
        // it contains all data needed for the test to run
        struct ITestFuncNode {
            std::string mFuncName;
            TestTags mTags;
            ITestFuncNode* mNext = nullptr; // link list next pointer

            // runs the test using the provided instance
            virtual TestResult run(ITestClass* aTestClassInstance) const = 0;

            // takes the function name and any tags that this test function uses
            // does not include tags from class
            ITestFuncNode(const std::string& aName, std::vector<std::string>&& aTags)
                : mFuncName(aName)
                , mTags(std::forward<std::vector<std::string>>(aTags)) {}
        };


        // -------------------------------------------------------------------------------
        // Templated version of ITestFuncNode for specific implementations
        template <typename tNodeClass, typename tClass>
        struct TestFuncNode final : public ITestFuncNode {
            // member function pointer to test
            void(tClass::* mTestFunc)();

            // takes the function name, any tags for the test function, and a member function pointer to the function itself
            TestFuncNode(const std::string& aName, std::vector<std::string>&& aTags, void(tClass::* aTestFunc)())
                : ITestFuncNode(aName, std::forward<std::vector<std::string>>(aTags))
                , mTestFunc(aTestFunc) {

                // handles the static link list update with this new node
                // static link list is in the TestClassNode<T> class
                mNext = tNodeClass::sFuncNodes;
                tNodeClass::sFuncNodes = this;
            }

            // runs the test and reports the result
            TestResult run(ITestClass* aTestClassInstance) const override {
                tClass* aTestClass = static_cast<tClass*>(aTestClassInstance); // upcast to proper class

                (aTestClass->*mTestFunc)(); // call test function

                if (aTestClass->mTestResult == TestResult::None) {
                    return TestResult::Pass; // test return void, so if not failed, then it passes
                }
                else {
                    TestResult result = aTestClass->mTestResult;
                    aTestClass->mTestResult = TestResult::None;
                    return result;
                }
            }
        };


        // ===========================================================================================================================
        // Assertion helpers for when detecting test failures
        struct Assert {
            // returns the filename of the file from compiler value __FILE__
            static std::string fileNameFromPath(const char* fileName) {
                std::string filename = fileName;
                size_t lastSlash = filename.size();
                for (auto it = filename.rbegin(); it != filename.rend(); ++it) {
#if OP_PLATFORM_WINDOWS
                    if (*it == '\\') {
#else
                    if (*it == '/') {
#endif
                        break;
                    }
                    lastSlash--;
                }
                if (lastSlash != 0) {
                    return filename.substr(lastSlash);
                }
                else {
                    return filename;
                }
            }

            // converts to bool instead of using !! or static casts
            static bool toBool(bool aExpressionOutcome) {
                return aExpressionOutcome;
            }

            // formats a string using the '{}' style formatting
            template <typename... tArgs>
            static std::string _format(const std::string& format, tArgs&&... args) {
                return std::vformat(format, std::make_format_args(args...));
            }

            template <typename... tArgs>
            static std::string tryFormat(tArgs&&... args) {
                if constexpr (sizeof...(tArgs) == 0) {
                    return "";
                }
                else {
                    return _format(std::forward<tArgs>(args)...);
                }
            }
        };
    }


    // ===========================================================================================================================
    // handles calling into the tests and reporting
    class TestRunner {
        // gathers the tests that should be run
        struct TestData {
            TestData(const Internal::ITestClassNode* aClassNode)
                : mClassNode(aClassNode) {}

            const Internal::ITestClassNode* mClassNode;
            std::vector<const Internal::ITestFuncNode*> mFuncNodes = {};
        };

        // runs filtered list of tests
        static TestRunResult _runTests(std::vector<TestData>&& testsToRun, size_t testCount) {
            if (testCount == 0) {
                return TestRunResult{ TestResult::None, 0, 0 }; // No tests to run, return inconclusive result
            }

            double totalTime = 0.0;
            size_t failedCount = 0;
            LogHook::testRunnerInit(testCount);
            TestResult ret = TestResult::Pass;
            for (auto&& testData : testsToRun) {
                Internal::ITestClass::mCurrClassName = &testData.mClassNode->mClassName;
                double classTime = 0.0;
                TestResult classResult = TestResult::Pass;
                LogHook::startClass(testData.mClassNode->mClassName);
                std::unique_ptr<Internal::ITestClass> testClass = testData.mClassNode->createClass();

                bool CanContinue = true;
                try {
                    testClass->onInit();
                }
                catch (Internal::FailException) {
                    // assert will already have logged
                    Internal::handleException({}, *testClass);
                    CanContinue = false;
                }
                catch (std::exception e) {
                    Internal::handleException(&e, *testClass);
                    CanContinue = false;
                }

                if (CanContinue) {
                    for (auto&& testFuncNode : testData.mFuncNodes) {
                        Internal::ITestClass::mCurrFuncTestName = &testFuncNode->mFuncName;
                        LogHook::startTest(testFuncNode->mFuncName);
                        bool CanTest = true;
                        try {
                            testClass->onTestStart();
                        }
                        catch (Internal::FailException) {
                            // assert will already have logged
                            Internal::handleException({}, *testClass);
                            CanTest = false;
                        }
                        catch (std::exception e) {
                            Internal::handleException(&e, *testClass);
                            CanTest = false;
                        }

                        util::HighResBenchmark<util::time::MilliSeconds> fnTimer;
                        TestResult result = TestResult::None;
                        if (CanTest) {
                            try {
                                result = testFuncNode->run(static_cast<Internal::ITestClass*>(testClass.get()));
                            }
                            catch (Internal::FailException) {
                                // assert will already have logged
                                Internal::handleException({}, *testClass);
                                result = TestResult::Fail;
                            }
                            catch (std::exception e) {
                                Internal::handleException(&e, *testClass);
                                result = TestResult::Fail;
                            }
                        }
                        double fnTime = fnTimer.elapsed();
                        classTime += fnTime;

                        try {
                            testClass->onTestStop();
                        }
                        catch (Internal::FailException) {
                            // assert will already have logged
                            Internal::handleException({}, *testClass);
                        }
                        catch (std::exception e) {
                            Internal::handleException(&e, *testClass);
                        }

                        if (result == TestResult::Fail) {
                            classResult = TestResult::Fail;
                            ret = TestResult::Fail;
                            ++failedCount;
                        }
                        // inconclusive results should be considered failures
                        else if (result != TestResult::Pass) {
                            classResult = TestResult::Fail;
                            ret = TestResult::Fail;
                            ++failedCount;
                        }
                        LogHook::endTest(testFuncNode->mFuncName, result, fnTime);
                        Internal::ITestClass::mCurrFuncTestName = nullptr;
                    }
                }

                try {
                    testClass->onDrop();
                }
                catch (Internal::FailException) {
                    // assert will already have logged
                    Internal::handleException({}, *testClass);
                }
                catch (std::exception e) {
                    Internal::handleException(&e, *testClass);
                }

                LogHook::endClass(testData.mClassNode->mClassName, classResult, classTime);
                totalTime += classTime / 1000.0; // convert to seconds by div by 1000
                testClass.reset();
                Internal::ITestClass::mCurrClassName = nullptr;
            }
            return TestRunResult{ ret, testCount, failedCount, totalTime };
        }

    public:
        // runs all but ignored tests
        static TestRunResult runAllTests() {
            std::unique_ptr<Internal::CoutLogHook> logger;
            if (!LogHook::hasHook()) {
                logger = std::make_unique<Internal::CoutLogHook>();
            }

            size_t testCount = 0;
            std::vector<TestData> testsToRun;
            Internal::ITestClassNode* testClassNode = Internal::TestInfraCollections::sTestClassListHead;
            while (testClassNode) {
                if (static_cast<char>(testClassNode->mTags.hasTag(Tags::Ignore)) > 0) {
                    testClassNode = testClassNode->mNext;
                    continue; // ignored tests cannot run in "run all tests" mode
                }
                testsToRun.emplace_back(TestData(testClassNode));
                Internal::ITestFuncNode* testFuncNode = testClassNode->getFuncList();
                while (testFuncNode) {
                    if (static_cast<char>(testFuncNode->mTags.hasTag(Tags::Ignore)) > 0) {
                        testFuncNode = testFuncNode->mNext;
                        continue; // ignored tests cannot run in "run all tests" mode
                    }
                    testsToRun.back().mFuncNodes.emplace_back(testFuncNode);
                    testFuncNode = testFuncNode->mNext;
                    testCount++;
                }
                testClassNode = testClassNode->mNext;
            }

            if (__TEST_INFRA_REVERSE_FN_ORDER) {
                for (TestData& data : testsToRun) {
                    std::reverse(data.mFuncNodes.begin(), data.mFuncNodes.end());
                }
            }
            return _runTests(std::move(testsToRun), testCount);
        }

        // if tag list is empty, runs all tests
        // runs all tests with the provided tags
        static TestRunResult runTests(std::vector<std::string>&& aTags) {
            if (aTags.empty()) {
                return runAllTests();
            }
            std::unique_ptr<Internal::CoutLogHook> logger;
            if (!LogHook::hasHook()) {
                logger = std::make_unique<Internal::CoutLogHook>();
            }

            size_t testCount = 0;
            std::vector<TestData> testsToRun;
            Internal::ITestClassNode* testClassNode = Internal::TestInfraCollections::sTestClassListHead;
            while (testClassNode) {
                testsToRun.emplace_back(TestData(testClassNode));
                if (testClassNode->mTags.hasTag(aTags)) {
                    Internal::ITestFuncNode* testFuncNode = testClassNode->getFuncList();
                    while (testFuncNode) {
                        testCount++;
                        testsToRun.back().mFuncNodes.emplace_back(testFuncNode);
                        testFuncNode = testFuncNode->mNext;
                    }
                    if (testsToRun.back().mFuncNodes.empty()) {
                        testsToRun.pop_back();
                    }
                }
                else {
                    Internal::ITestFuncNode* testFuncNode = testClassNode->getFuncList();
                    while (testFuncNode) {
                        if (testFuncNode->mTags.hasTag(aTags)) {
                            testsToRun.back().mFuncNodes.emplace_back(testFuncNode);
                            testCount++;
                        }
                        testFuncNode = testFuncNode->mNext;
                    }
                    if (testsToRun.back().mFuncNodes.empty()) {
                        testsToRun.pop_back();
                    }
                }
                testClassNode = testClassNode->mNext;
            }

            if (__TEST_INFRA_REVERSE_FN_ORDER) {
                for (TestData& data : testsToRun) {
                    std::reverse(data.mFuncNodes.begin(), data.mFuncNodes.end());
                }
            }
            return _runTests(std::move(testsToRun), testCount);
        }
    };
}


// ===========================================================================================================================
// This can just make a static variable in the file to initialize the node
#define TEST_CLASS(_className, ...) \
    class _className; \
    TestInfra::Internal::TestClassNode<_className> __TestInfra_ClassNode_ ## _className = TestInfra::Internal::TestClassNode<_className>(#_className, std::vector<std::string>{ #_className __VA_OPT__(, __VA_ARGS__) }); \
    class _className : public TestInfra::Internal::TestClass<_className>

// Because tests are inside a class, they dont auto initialize and need a separate thing
#define TEST(_testName, ...) \
    void __TestInfra_TempFunc_ ## _testName() { _testName(); } \
    friend struct TestInfra::Internal::TestFuncNode<ClassNodeType, ClassType>; \
    static inline TestInfra::Internal::TestFuncNode<ClassNodeType, ClassType> __TestInfra_FuncNode_ ## _testName = \
        TestInfra::Internal::TestFuncNode<ClassNodeType, ClassType>(#_testName, std::vector<std::string>{ #_testName __VA_OPT__(, __VA_ARGS__) }, &ClassType::__TestInfra_TempFunc_ ## _testName); \
    void _testName()

#define TEST_CLASS_INIT() void _onInit() override
#define TEST_CLASS_DROP() void _onDrop() override
#define TEST_START() void _onTestStart() override
#define TEST_STOP() void _onTestStop() override


// ===========================================================================================================================
// for use internally only
#ifdef OP_DEBUG
#define __TEST_INFRA_EXPECTTRUE(_expressionOutcome, _expressionString, _shouldStop, ...) \
    if (!TestInfra::Internal::Assert::toBool(_expressionOutcome)) { \
        TestInfra::LogHook::error("Assertion Failure:\n\t\tFilename: {}\n\t\tClass: {}\n\t\tFunction: {}\n\t\tLine Number: {}\n\t\tAssertion: {}\n\t\tMessage: {}", \
            TestInfra::Internal::Assert::fileNameFromPath(__FILE__), \
            TestInfra::Internal::ITestClass::mCurrClassName ? *TestInfra::Internal::ITestClass::mCurrClassName : "UnknownClassName", \
            TestInfra::Internal::ITestClass::mCurrFuncTestName ? *TestInfra::Internal::ITestClass::mCurrFuncTestName : "UnknownFuncName", \
            __LINE__, \
            _expressionString, \
            TestInfra::Internal::Assert::tryFormat(__VA_OPT__(__VA_ARGS__)) \
        ); \
        TestInfra::Internal::ITestClass::mTestResult = TestInfra::TestResult::Fail; \
        if (__TEST_INFRA_BREAK_ON_FAIL) { \
            pdebugbreak(); \
        } \
        if (_shouldStop) { \
            throw TestInfra::Internal::FailException(); \
        } \
    }
#else
#define __TEST_INFRA_EXPECTTRUE(_expressionOutcome, _expressionString, _shouldStop, ...) \
    if (!TestInfra::Internal::Assert::toBool(_expressionOutcome)) { \
        TestInfra::LogHook::error("Assertion Failure:\n\t\tFilename: {}\n\t\tClass: {}\n\t\tFunction: {}\n\t\tLine Number: {}\n\t\tAssertion: {}\n\t\tMessage: {}", \
            TestInfra::Internal::Assert::fileNameFromPath(__FILE__), \
            TestInfra::Internal::ITestClass::mCurrClassName ? *TestInfra::Internal::ITestClass::mCurrClassName : "UnknownClassName", \
            TestInfra::Internal::ITestClass::mCurrFuncTestName ? *TestInfra::Internal::ITestClass::mCurrFuncTestName : "UnknownFuncName", \
            __LINE__, \
            _expressionString, \
            TestInfra::Internal::Assert::tryFormat(__VA_OPT__(__VA_ARGS__)) \
        ); \
        TestInfra::Internal::ITestClass::mTestResult = TestInfra::TestResult::Fail; \
        if (__TEST_INFRA_BREAK_ON_FAIL) { \
            assert(false); \
        } \
        if (_shouldStop) { \
            throw TestInfra::Internal::FailException(); \
        } \
    }
#endif


// -------------------------------------------------------------------------------
// EXPECT macros will not crash the engine, they will report the errors though
#define EXPECT_TRUE(_expression, ...) __TEST_INFRA_EXPECTTRUE(_expression, #_expression, false __VA_OPT__(, __VA_ARGS__));
#define EXPECT_FALSE(_expression, ...) __TEST_INFRA_EXPECTTRUE(!(_expression), #_expression, false __VA_OPT__(, __VA_ARGS__));
#define EXPECT_FAIL(...) __TEST_INFRA_EXPECTTRUE(false, "FAIL", false __VA_OPT__(, __VA_ARGS__));
#define EXPECT_EQ(_expected, _actual, ...) EXPECT_TRUE((_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NEQ(_expected, _actual, ...) EXPECT_FALSE((_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_LTE(_expected, _actual, ...) EXPECT_TRUE((_expected) < (_actual) || (_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_LT(_expected, _actual, ...) EXPECT_TRUE((_expected) < (_actual) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_GTE(_expected, _actual, ...) EXPECT_FALSE((_actual) < (_expected) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_GT(_expected, _actual, ...) EXPECT_FALSE((_actual) < (_expected) || (_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NEAR(_expected, _actual, _epsilon, ...) EXPECT_TRUE(std::abs((_actual) - (_expected)) < (_epsilon) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NOTNEAR(_expected, _actual, _epsilon, ...) EXPECT_FALSE(std::abs((_actual) - (_expected)) < (_epsilon) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NULL(_actual, ...) EXPECT_TRUE((_actual) == nullptr __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NOTNULL(_actual, ...) EXPECT_FALSE((_actual) == nullptr __VA_OPT__(, __VA_ARGS__));
#define EXPECT_STR_NOCASE_EQ(_expected, _actual, ...) EXPECT_TRUE(std::tolower(_actual) == std::tolower(_expected) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_EMPTY(_value, ...) EXPECT_FALSE((_value.has_value()) __VA_OPT__(, __VA_ARGS__));
#define EXPECT_NEMPTY(_value, ...) EXPECT_TRUE((_value.has_value()) __VA_OPT__(, __VA_ARGS__));

// Example: EXPECT_THROW(
//     Do something in the try group... ,
//     catch(someException e) { your handling... },
//     "Message if this fails");
#define EXPECT_THROW(_action, _expectedCatchExpressionAndHandling, ...) \
    bool didThrow = true; \
    try { \
        _action; \
        didThrow = false; \
    } _expectedCatchExpressionAndHandling \
    catch (...) { \
        EXPECT_FAIL("Unknown exception took place: " __VA_OPT__(, __VA_ARGS__)); \
    } \
    EXPECT_TRUE(didThrow, "Expected an exception, none occurred: " __VA_OPT__(, __VA_ARGS__));

#define EXPECT_ANYTHROW(_action, ...) \
    bool didThrow = true; \
    try { \
        _action; \
        didThrow = false; \
    } \
    catch (...) { \
        didThrow = true; \
    } \
    EXPECT_TRUE(didThrow, "Expected an exception, none occurred: " __VA_OPT__(, __VA_ARGS__));

#define EXPECT_NOTHROW(_action, ...) \
    try { \
        _action; \
    } catch(...) { \
        EXPECT_FAIL("Expected no exception, an exception has occurred: " __VA_OPT__(, __VA_ARGS__)); \
    }


// -------------------------------------------------------------------------------
// ASSERT macros WILL crash the engine after reporting the errors
#define ASSERT_TRUE(_expression, ...) __TEST_INFRA_EXPECTTRUE(_expression, #_expression, true __VA_OPT__(, __VA_ARGS__));
#define ASSERT_FALSE(_expression, ...) __TEST_INFRA_EXPECTTRUE(!(_expression), #_expression, true __VA_OPT__(, __VA_ARGS__));
#define ASSERT_FAIL(...) __TEST_INFRA_EXPECTTRUE(false, "FAIL", true __VA_OPT__(, __VA_ARGS__));
#define ASSERT_EQ(_expected, _actual, ...) ASSERT_TRUE((_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NEQ(_expected, _actual, ...) ASSERT_FALSE((_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_LTE(_expected, _actual, ...) ASSERT_TRUE((_expected) < (_actual) || (_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_LT(_expected, _actual, ...) ASSERT_TRUE((_expected) < (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_GTE(_expected, _actual, ...) ASSERT_FALSE((_expected) < (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_GT(_expected, _actual, ...) ASSERT_FALSE((_expected) < (_actual) || (_expected) == (_actual) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NEAR(_expected, _actual, _epsilon, ...) ASSERT_TRUE(Math::abs((_actual) - (_expected)) < (_epsilon) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NOTNEAR(_expected, _actual, _epsilon, ...) ASSERT_FALSE(Math::abs((_actual) - (_expected)) < (_epsilon) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NULL(_actual, ...) ASSERT_TRUE((_actual) == nullptr __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NOTNULL(_actual, ...) ASSERT_FALSE((_actual) == nullptr __VA_OPT__(, __VA_ARGS__));
#define ASSERT_STR_NOCASE_EQ(_expected, _actual, ...) ASSERT_TRUE(std::tolower(_actual) == std::tolower(_expected) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_EMPTY(_value, ...) ASSERT_FALSE((_value.has_value()) __VA_OPT__(, __VA_ARGS__));
#define ASSERT_NEMPTY(_value, ...) ASSERT_TRUE((_value.has_value()) __VA_OPT__(, __VA_ARGS__));

#define ASSERT_THROW(_action, _expectedCatchExpressionAndHandling, ...) \
    bool didThrow = true; \
    try { \
        _action; \
        didThrow = false; \
    } _expectedCatchExpressionAndHandling \
    catch (...) { \
        ASSERT_FAIL("Unknown exception took place: "  __VA_OPT__(+ __VA_ARGS__)); \
    } \
    ASSERT_TRUE(didThrow, "Expected an exception, none occurred: "  __VA_OPT__(+ __VA_ARGS__));

#define ASSERT_ANYTHROW(_action, ...) \
    bool didThrow = true; \
    try { \
        _action; \
        didThrow = false; \
    } \
    catch (...) { \
        didThrow = true; \
    } \
    ASSERT_TRUE(didThrow, "Expected an exception, none occurred: "  __VA_OPT__(+ __VA_ARGS__));

#define ASSERT_NOTHROW(_action, ...) \
    try { \
        _action; \
    } catch(...) { \
        ASSERT_FAIL("Expected no exception, an exception has occurred: "  __VA_OPT__(+ __VA_ARGS__)); \
    }

#endif // TEST_ENABLED
