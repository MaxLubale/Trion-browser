#include "include/cef_app.h"          // Include the main CEF header
#include "include/cef_browser.h"      // Include the CEF browser header
#include "include/cef_client.h"       // Include the CEF client header
#include "include/wrapper/cef_helpers.h" // Include CEF helper functions
#include "include/wrapper/cef_library_loader.h" // Include CEF library loader

// Custom handler class derived from CefClient and CefLifeSpanHandler
class SimpleHandler : public CefClient, public CefLifeSpanHandler {
public:
    SimpleHandler() {} // Constructor

    // Override to provide the CefLifeSpanHandler implementation
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this; // Return the current object as the life span handler
    }

    // Called after a new browser window is created
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        CEF_REQUIRE_UI_THREAD(); // Ensure this method is called on the UI thread
        browser_list_.push_back(browser); // Add the browser to the list of open browsers
    }

    // Called when the browser is about to be closed
    bool DoClose(CefRefPtr<CefBrowser> browser) override {
        CEF_REQUIRE_UI_THREAD(); // Ensure this method is called on the UI thread
        if (browser_list_.size() == 1) {
            // If this is the last browser window, allow it to close
            return false;
        }
        // Otherwise, cancel the close
        return true;
    }

    // Called after a browser has been closed
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        CEF_REQUIRE_UI_THREAD(); // Ensure this method is called on the UI thread
        // Remove the browser from the list of open browsers
        BrowserList::iterator bit = browser_list_.begin();
        for (; bit != browser_list_.end(); ++bit) {
            if ((*bit)->IsSame(browser)) {
                browser_list_.erase(bit); // Erase the browser from the list
                break;
            }
        }
        // If no browsers are open, quit the message loop
        if (browser_list_.empty()) {
            CefQuitMessageLoop();
        }
    }

private:
    // List to keep track of open browsers
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList browser_list_;

    // Include the default reference counting implementation
    IMPLEMENT_REFCOUNTING(SimpleHandler);
};

// Custom application class derived from CefApp and CefBrowserProcessHandler
class SimpleApp : public CefApp, public CefBrowserProcessHandler {
public:
    // Override to provide the CefBrowserProcessHandler implementation
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this; // Return the current object as the browser process handler
    }

    // Called when the CEF context is initialized
    void OnContextInitialized() override {
        CEF_REQUIRE_UI_THREAD(); // Ensure this method is called on the UI thread

        CefBrowserSettings browser_settings; // Create browser settings
        CefWindowInfo window_info; // Create window information
        window_info.SetAsPopup(NULL, "SimpleBrowser"); // Set the window as a popup

        // Create a new browser instance
        CefRefPtr<SimpleHandler> handler(new SimpleHandler());
        CefBrowserHost::CreateBrowser(window_info, handler.get(), "http://www.example.com", browser_settings, nullptr, nullptr);
    }

private:
    // Include the default reference counting implementation
    IMPLEMENT_REFCOUNTING(SimpleApp);
};

// Main entry point for the application
int main(int argc, char* argv[]) {
    CefEnableHighDPISupport(); // Enable High DPI support
    CefMainArgs main_args(argc, argv); // Create CEF main arguments

    // Load the CEF library
    CefScopedLibraryLoader library_loader;
    if (!library_loader.LoadInMain()) {
        return 1;
    }

    // Create CEF application settings
    CefSettings settings;
    settings.no_sandbox = true; // Disable sandbox for simplicity

    // Create the CEF application instance
    CefRefPtr<SimpleApp> app(new SimpleApp());

    // Initialize CEF with the specified settings and application instance
    int exit_code = CefExecuteProcess(main_args, app, nullptr);
    if (exit_code >= 0) {
        return exit_code;
    }

    CefInitialize(main_args, settings, app, nullptr); // Initialize CEF

    // Run the CEF message loop
    CefRunMessageLoop();

    // Shut down CEF
    CefShutdown();

    return 0;
}
