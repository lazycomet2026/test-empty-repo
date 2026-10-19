package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.webkit.*;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends Activity {
    private WebView webView;
    private String[] ipList = {"140.82.114.3", "20.205.243.166", "140.82.112.4", "140.82.113.4"};
    private int currentIpIndex = 0;
    
    // 【核心】伪装成 Windows 电脑端的 Chrome 浏览器
    private static final String DESKTOP_UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 强制使用电脑端 UA
        settings.setUserAgentString(DESKTOP_UA);
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                
                // 放行 POST 请求（防止登录卡死）
                if (!"GET".equals(request.getMethod())) {
                    return super.shouldInterceptRequest(view, request);
                }

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), ipList[currentIpIndex], originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        
                        conn.setConnectTimeout(8000);
                        conn.setReadTimeout(8000);
                        
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        // 【核心】请求头里也带上电脑端 UA
                        conn.setRequestProperty("User-Agent", DESKTOP_UA);
                        
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
                    } catch (Exception e) {
                        // 失败放行
                    }
                }
                return super.shouldInterceptRequest(view, request);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (request.isForMainFrame()) {
                    if (currentIpIndex < ipList.length - 1) {
                        currentIpIndex++;
                        webView.loadUrl("https://github.com");
                    } else {
                        runOnUiThread(() -> {
                            new AlertDialog.Builder(MainActivity.this)
                                    .setTitle("连接超时")
                                    .setMessage("当前所有节点均不可用，请检查网络或稍后再试。")
                                    .setPositiveButton("重试", (dialog, which) -> {
                                        currentIpIndex = 0;
                                        webView.loadUrl("https://github.com");
                                    })
                                    .setNegativeButton("退出", (dialog, which) -> finish())
                                    .show();
                        });
                    }
                }
            }
        });
        
        webView.loadUrl("https://github.com");
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
