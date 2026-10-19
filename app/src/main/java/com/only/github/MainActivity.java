package com.only.github;
import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class MainActivity extends Activity {
    private WebView webView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        webView = new WebView(this);
        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        webView.setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url.replace("github.com", "20.205.243.166"));
                return true;
            }
        });
        webView.loadUrl("https://20.205.243.166");
        setContentView(webView);
    }
    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) { webView.goBack(); } else { super.onBackPressed(); }
    }
}
