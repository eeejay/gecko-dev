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
        File Name:      number-001.js
        Description:

        Accessing a Java field whose value is one of the primitive Java types
        below, JavaScript should read this as a JavaScript Number object.
            byte
            short
            int
            long
            float
            double
            char

        To test this:
        1.  Instantiate a new Java object that has a field whose type one of
            the above primitive Java types, OR get the value of a class's static
            field.
        2.  Check the value of the returned object
        3.  Check the type of the returned object, which should be "object"
        4.  Check the class of the returned object using Object.prototype.toString,
            which should return "[object Number]"

        It is an error if the type of the JavaScript variable is "number" or if
        its class is JavaObject.

        @author     christine@netscape.com
        @version    1.00
*/
    var SECTION = "LiveConnect";
    var VERSION = "1_3";
    var TITLE   = "Java Number Primitive to JavaScript Object";

    var testcases = new Array();

    startTest();
    writeHeaderToLog( SECTION + " "+ TITLE);

    //  In all test cases, the expected type is "object, and the expected
    //  class is "Number"

    var E_TYPE = "number";

    //  Create arrays of actual results (java_array) and expected results
    //  (test_array).

    var java_array = new Array();
    var test_array = new Array();

    var i = 0;

    //  Get a static java field whose type is byte.

    java_array[i] = new JavaValue(  java.lang.Byte.MIN_VALUE );
    test_array[i] = new TestValue(  "java.lang.Byte.MIN_VALUE",
                                    -128 )
    i++;

    // Get a static java field whose type is short.
    java_array[i] = new JavaValue(  java.lang.Short.MIN_VALUE );
    test_array[i] = new TestValue(  "java.lang.Short.MIN_VALUE",
                                    -32768 )
    i++;

    //  Get a static java field whose type is int.

    java_array[i] = new JavaValue( java.lang.Integer.MIN_VALUE );
    test_array[i] = new TestValue( "java.lang.Integer.MIN_VALUE",
                                   -2147483648 )
    i++;


    //  Instantiate a class, and get a field in that class whose type is int.

    var java_rect = new java.awt.Rectangle( 1,2,3,4 );

    java_array[i] = new JavaValue( java_rect.width );
    test_array[i] = new TestValue( "java_object = new java.awt.Rectangle( 1,2,3,4 ); java_object.width",
                                   3 );
    i++;

    //  Get a static java field whose type is long.
    java_array[i] = new JavaValue(  java.lang.Long.MIN_VALUE );
    test_array[i] = new TestValue(  "java.lang.Long.MIN_VALUE",
                                    -9223372036854776000 );
    i++;

    //  Get a static java field whose type is float.
    java_array[i] = new JavaValue(  java.lang.Float.MAX_VALUE );
    test_array[i] = new TestValue(  "java.lang.Float.MAX_VALUE",
                                     3.4028234663852886e+38 )
    i++;

    //  Get a static java field whose type is double.
    java_array[i] = new JavaValue(  java.lang.Double.MAX_VALUE );
    test_array[i] = new TestValue(  "java.lang.Double.MAX_VALUE",
                                     1.7976931348623157e+308 )
    i++;

    //  Get a static java field whose type is char.
    java_array[i] = new JavaValue(  java.lang.Character.MAX_VALUE );
    test_array[i] = new TestValue(  "java.lang.Character.MAX_VALUE",
                                     65535 );
    i++;

    for ( i = 0; i < java_array.length; i++ ) {
        CompareValues( java_array[i], test_array[i] );

    }

    test();
function CompareValues( javaval, testval ) {
    //  Check value
    testcases[testcases.length] = new TestCase( SECTION,
                                                testval.description,
                                                testval.value,
                                                javaval.value );
    //  Check type.

    testcases[testcases.length] = new TestCase( SECTION,
                                                "typeof (" + testval.description +")",
                                                testval.type,
                                                javaval.type );
}
function JavaValue( value ) {
    this.value  = value.valueOf();
    this.type   = typeof value;
    return this;
}
function TestValue( description, value, type  ) {
    this.description = description;
    this.value = value;
    this.type =  E_TYPE;
//    this.classname = classname;
    return this;
}
function test() {
    for ( tc=0; tc < testcases.length; tc++ ) {
        testcases[tc].passed = writeTestCaseResult(
                            testcases[tc].expect,
                            testcases[tc].actual,
                            testcases[tc].description +" = "+
                            testcases[tc].actual );

        testcases[tc].reason += ( testcases[tc].passed ) ? "" : "wrong value ";
    }
    stopTest();
    return ( testcases );
}
