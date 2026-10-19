package com.only.github;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.webkit.*;
import android.widget.Toast;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends Activity {
    private WebView webView;
    private String currentTargetIp = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 【核心】开启 Cookie 持久化（记住登录）
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);

        // 注册 JS 桥接，让前端 HTML 能调用 Java 方法
        webView.addJavascriptInterface(new Object() {
            @android.webkit.JavascriptInterface
            public void loadTarget(String ip) {
                runOnUiThread(() -> {
                    currentTargetIp = ip;
                    // 这里我们直接加载 GitHub
                    webView.loadUrl("https://github.com");
                });
            }
        }, "AndroidBridge");

        webView.setWebViewClient(new WebViewClient() {
            // 在这里劫持所有请求，把 github.com 替换成我们在前端测速选出来的 IP
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                if (!currentTargetIp.isEmpty() && url.contains("github.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), currentTargetIp, originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("User-Agent", view.getSettings().getUserAgentString());
                        
                        // 把 Cookie 带过去
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) {
                            conn.setRequestProperty("Cookie", cookie);
                        }
                        
                        InputStream is = conn.getInputStream();
                        String contentType = conn.getContentType();
                        String encoding = conn.getContentEncoding();
                        if (contentType == null) contentType = "text/html";
                        if (encoding == null) encoding = "utf-8";
                        return new WebResourceResponse(contentType, encoding, is);
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }
        });
        
        // 页面关闭时同步 Cookie
        cookieManager.flush();
        
        // 加载我们本地的启动页面
        webView.loadUrl("file:///android_asset/index.html");
        setContentView(webView);
    }

    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
