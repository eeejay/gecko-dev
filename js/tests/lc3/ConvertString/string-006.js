/* The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code, released March
 * 31, 1998.
 *
 * The Initial Developer of the Original Code is Netscape Communications
 * Corporation. Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 * 
 */
/**
 *  Preferred Argument Conversion.
 *
 *  Pass a JavaScript number to ambiguous method.  Use the explicit method
 *  invokation syntax to override the preferred argument conversion.
 *
 */
    var SECTION = "Preferred argument conversion: string";
    var VERSION = "1_4";
    var TITLE   = "LiveConnect 3.0 JavaScript to Java Data Type Conversion " +
                    SECTION;
    startTest();

    var TEST_CLASS = new
        Packages.com.netscape.javascript.qa.lc3.string.String_001;

    var string = "255";

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(java.lang.String)\"](string))",
        "STRING",
        TEST_CLASS["ambiguous(java.lang.String)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(java.lang.Object)\"](string))",
        "OBJECT",
        TEST_CLASS["ambiguous(java.lang.Object)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(char)\"](string))",
        "CHAR",
        TEST_CLASS["ambiguous(char)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(double)\"](string))",
        "DOUBLE",
        TEST_CLASS["ambiguous(double)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(float)\"](string))",
        "FLOAT",
        TEST_CLASS["ambiguous(float)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(long)\"](string))",
        "LONG",
        TEST_CLASS["ambiguous(long)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(int)\"](string))",
        "INT",
        TEST_CLASS["ambiguous(int)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(short)\"](string))",
        "SHORT",
        TEST_CLASS["ambiguous(short)"](string) +'');

    testcases[testcases.length] = new TestCase(
        "TEST_CLASS[\"ambiguous(byte)\"](\"127\"))",
        "BYTE",
        TEST_CLASS["ambiguous(byte)"]("127") +'');


    test();