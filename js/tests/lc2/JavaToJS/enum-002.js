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
    File Name:          enum-002.js
    Section:            LiveConnect
    Description:

    Tests enumeration of a java object.  This also does some
    conversions.

    Regression test for bug:

    http://scopus.mcom.com/bugsplat/show_bug.cgi?id=107638

    Author:             christine@netscape.com
    Date:               12 november 1997
*/
    var SECTION = "enum-002";
    var VERSION = "JS1_3";
    var TITLE   = "The variable statment";

    startTest();
    writeHeaderToLog( SECTION + " "+ TITLE);

    var testcases = new Array();

    var v = new java.util.Vector();
    v.addElement("TRUE");

    for (e = v.elements(), result = new Array(), i = 0 ; e.hasMoreElements();
        i++ )
    {
        result[i] = (new java.lang.Boolean(e.nextElement())).booleanValue();
    }

    for ( i = 0; i < result.length; i++ ) {
        testcases[testcases.length] = new TestCase( SECTION,
            "test enumeration of a java object:  element at " + i,
            "true",
            String( result[i] ) );
    }

    test();

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
