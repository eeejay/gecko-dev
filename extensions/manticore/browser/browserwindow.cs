/* -*- Mode: C#; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/ 
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License. 
 *
 * The Original Code is Manticore.
 * 
 * The Initial Developer of the Original Code is
 * Silverstone Interactive. Portions created by Silverstone Interactive are
 * Copyright (C) 2001 Silverstone Interactive. 
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU Public License (the "GPL"), in which case the
 * provisions of the GPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only
 * under the terms of the GPL and not to allow others to use your
 * version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL.  If you do not delete
 * the provisions above, a recipient may use your version of this
 * file under either the MPL or the GPL.
 *
 * Contributor(s):
 *  Ben Goodger <ben@netscape.com>
 *
 */

namespace Silverstone.Manticore.Browser
{
  using System;
  using System.ComponentModel;
  using System.Drawing;
  using System.Windows.Forms;

  using Silverstone.Manticore.App;
  using Silverstone.Manticore.Toolkit;
  using Silverstone.Manticore.Layout;

  public class BrowserWindow : Form 
  {
    private System.ComponentModel.Container components;

    private BrowserMenuBuilder mMenuBuilder;
    private BrowserToolbarBuilder mToolbarBuilder;
    private BrowserCommandHandler mCommandHandler;

    protected internal WebBrowser webBrowser;

    protected internal StatusBar mStatusBar;

    protected internal ManticoreApp application;

    public BrowserWindow(ManticoreApp app)
    {
      application = app;

      Console.WriteLine("init component");
      // Set up UI
      InitializeComponent();
    }

    public override void Dispose()
    {
      base.Dispose();
      components.Dispose();
    }

    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();

      // XXX read these from a settings file
      this.Width = 640;
      this.Height = 480;
      
      this.Text = "Manticore"; // XXX localize

      mCommandHandler = new BrowserCommandHandler(this);

      mMenuBuilder = new BrowserMenuBuilder("browser\\browser-menu.xml", mCommandHandler);
      mMenuBuilder.Build();
      this.Menu = mMenuBuilder.mainMenu;
      
      // Show the resize handle
      this.SizeGripStyle = SizeGripStyle.Auto;

      // Set up the Status Bar
      mStatusBar = new StatusBar();
      
      StatusBarPanel docStatePanel = new StatusBarPanel();
      StatusBarPanel statusPanel = new StatusBarPanel();
      StatusBarPanel progressPanel = new StatusBarPanel();
      StatusBarPanel zonePanel = new StatusBarPanel();

      docStatePanel.Text = "X";
      progressPanel.Text = "[|||||         ]";
      zonePanel.Text = "Internet Region";
      statusPanel.Text = "Document Done";
      statusPanel.AutoSize = StatusBarPanelAutoSize.Spring;
      

      mStatusBar.Panels.AddRange(new StatusBarPanel[] {docStatePanel, statusPanel, progressPanel, zonePanel});
      mStatusBar.ShowPanels = true;

      webBrowser = new WebBrowser(this);
      this.Controls.Add(webBrowser);

      this.Controls.Add(mStatusBar);
      
	    mToolbarBuilder = new BrowserToolbarBuilder("browser\\browser-toolbar.xml", mCommandHandler, this);
	    mToolbarBuilder.Build();

      // Start Page handler
      this.VisibleChanged += new EventHandler(LoadStartPage);
    }

    private void LoadStartPage(object sender, EventArgs e)
    {
      int startMode = application.Prefs.GetIntPref("browser.homepage.mode");
      switch (startMode) {
      case 0:
        // Don't initialize jack
        break;
      case 1:
        // Load the homepage
        String homepageURL = application.Prefs.GetStringPref("browser.homepage");
        webBrowser.LoadURL(homepageURL, false);
        break;
      case 2:
        // Load the last page visited.
        // XXX todo
        break;
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Menu Command Handlers
    public void OpenNewBrowser()
    {
      application.OpenNewBrowser();
    }

    public void Open()
    {
      OpenDialog dlg = new OpenDialog();
      if (dlg.ShowDialog() == DialogResult.OK)
        webBrowser.LoadURL(dlg.URL, false);
    }
  }

  public class BrowserCommandHandler
  {
    private BrowserWindow mBrowserWindow;
    
    public BrowserCommandHandler(BrowserWindow window)
    {
      mBrowserWindow = window;
    }

    public void DoCommand(String s) {
      switch (s) 
      {
        case "file-new-window":
          mBrowserWindow.OpenNewBrowser();
          break;
        case "file-open":
          mBrowserWindow.Open();
          break;
        case "view-go-back":
          mBrowserWindow.webBrowser.GoBack();
          break;
        case "view-go-forward":
          mBrowserWindow.webBrowser.GoForward();
          break;
        case "view-go-home":
          mBrowserWindow.webBrowser.GoHome();
          break;
        case "view-reload":
          mBrowserWindow.webBrowser.RefreshPage();
          break;
        case "view-stop":
          mBrowserWindow.webBrowser.Stop();
          break;
        case "view-layout-gecko":
          mBrowserWindow.webBrowser.SwitchLayoutEngine("gecko");
          break;
        case "view-layout-ie":
          mBrowserWindow.webBrowser.SwitchLayoutEngine("trident");
          break;
        case "help-about":
          AboutDialog aboutDialog = new AboutDialog(mBrowserWindow);
          aboutDialog.ShowDialog();
          break;
        case "tools-options":
          PrefsDialog prefsDialog = new PrefsDialog(mBrowserWindow);
          prefsDialog.ShowDialog();
          break;
      }
    }
  }

  public class BrowserMenuBuilder : MenuBuilder
  {
    private BrowserCommandHandler mCommandHandler;

    public BrowserMenuBuilder(String file, BrowserCommandHandler handler) : base(file)
    {
      mCommandHandler = handler;
    }

    public override void OnCommand(Object sender, EventArgs e)
    {
      CommandMenuItem item = sender as CommandMenuItem;
      mCommandHandler.DoCommand(item.Command);
    }
  }

  public class BrowserToolbarBuilder : ToolbarBuilder
  {
    private BrowserCommandHandler mCommandHandler;

    public BrowserToolbarBuilder(String file, BrowserCommandHandler handler, 
                                 BrowserWindow window) : base(file, window)
    {
      mCommandHandler = handler;
    }

    public override void OnCommand(Object sender, ToolBarButtonClickEventArgs e)
    {
      CommandButtonItem item = e.Button as CommandButtonItem;
      mCommandHandler.DoCommand(item.Command);
    }
  }
}


