
namespace Silverstone.Manticore.App 
{
  using System;
  using System.ComponentModel;
  using System.Windows.Forms;
  using System.Collections;

  using Silverstone.Manticore.Browser;
  using Silverstone.Manticore.Core;
  using Silverstone.Manticore.Bookmarks;

  public class ManticoreApp
  {
    // XXX Need to do something here more similar
    //     to what mozilla does here for parameterized
    //     window types.
    private Queue mBrowserWindows;
    private Preferences mPreferences;
    private Bookmarks mBookmarks;

    public Preferences Prefs {
      get {
        return mPreferences;
      }
    }

    public Bookmarks BM {
      get {
        return mBookmarks;
      }
    }

    public ManticoreApp()
    {
      mBrowserWindows = new Queue();

      // Initialize default and user preferences
      mPreferences = new Preferences();
      mPreferences.InitializeDefaults("default-prefs.xml");
      mPreferences.LoadPreferencesFile("user-prefs.xml");

      // Initialize bookmarks
      mBookmarks = new Bookmarks(this);
      mBookmarks.LoadBookmarks();
      
      OpenNewBrowser();

      Application.Run();
    }

    ~ManticoreApp()
    {
      // Flush preferences to disk.
      mPreferences.FlushPreferencesFile("user-prefs.xml");
    }

    // Opens and displays a new browser window
    public void OpenNewBrowser()
    {
      BrowserWindow window = new BrowserWindow(this);
      mBrowserWindows.Enqueue(window);
      window.Show();
    }

    public static void Main(string[] args) 
    {
      ManticoreApp app = new ManticoreApp();
    }
  }
}

