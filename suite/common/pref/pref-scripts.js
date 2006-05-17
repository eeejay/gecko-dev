/* -*- Mode: Java; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla.org Code.
 *
 * The Initial Developer of the Original Code is
 * Doron Rosenberg.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Jonas J�rgensen <jonasj@jonasj.dk>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

function changeDisabledState(state){
  //Set the states of the groupbox children state based on the "javascript enabled" checkbox value
  document.getElementById("allowScripts").disabled = state;
  document.getElementById("allowWindowMoveResize").disabled = state;
  document.getElementById("allowImageSrcChange").disabled = state;
  document.getElementById("allowWindowStatusChange").disabled = state;
  document.getElementById("allowWindowFlip").disabled = state;
  document.getElementById("allowHideStatusBar").disabled = state;
}

function javascriptEnabledChange(){
  // if javascriptAllowMailNews is overlayed (mailnews is installed), then if javascriptAllowMailnews 
  // and javascriptAllowNavigator are unchecked, we disable the tree items. 
  // If javascriptAllowMailNews is not available, we only take javascriptAllowNavigator in consideration

  if (document.getElementById('javascriptAllowMailNews')){
    if (!document.getElementById('javascriptAllowNavigator').checked && !document.getElementById('javascriptAllowMailNews').checked)
      changeDisabledState(true);
    else changeDisabledState(false);
  } else {
    changeDisabledState(!document.getElementById('javascriptAllowNavigator').checked);
  }
}

function Startup(){
  //If we don't have a checkbox under groupbox pluginPreferences, we should hide it
  var pluginGroup = document.getElementById("pluginPreferences");
  var children = pluginGroup.childNodes;
  if (!children || children.length <= 1)    // 1 for the caption
    pluginGroup.setAttribute("hidden", "true");

  javascriptEnabledChange();
}
