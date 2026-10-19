package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.webkit.*;
import android.content.Intent;
import android.net.Uri;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends Activity {
    private WebView webView;
    private String[] ipList = {"140.82.114.3", "20.205.243.166", "140.82.112.4", "140.82.113.4"};
    private int currentIpIndex = 0;

    // 【核心】使用真正的移动端 UA（伪装成 Pixel 手机 Chrome），GitHub 会自动下发极简的移动版 UI
    private static final String MOBILE_UA = "Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 关键修复：使用移动端 UA，并且不要开启宽视图
        settings.setUserAgentString(MOBILE_UA);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        
        // 允许 JS 自动弹出新窗口（防止某些链接点了没反应）
        settings.setSupportMultipleWindows(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        // 【新增】配置 WebChromeClient，接管弹窗和跳转，解决点击无响应
        webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onCreateWindow(WebView view, boolean isDialog, boolean isUserGesture, android.os.Message resultMsg) {
                // 拦截 target="_blank" 的新窗口请求，使其在当前 WebView 加载
                WebView.HitTestResult result = view.getHitTestResult();
                String data = result.getExtra();
                if (data != null) {
                    webView.loadUrl(data);
                }
                return false;
            }
        });

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                
                // 1. 放行 POST/OPTIONS 等非 GET 请求，防止登录和 API 调用死锁
                if (!"GET".equals(request.getMethod())) {
                    return super.shouldInterceptRequest(view, request);
                }

                // 2. 只拦截主域名，并且带上正确的移动端请求头
                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), ipList[currentIpIndex], originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        
                        conn.setConnectTimeout(8000);
                        conn.setReadTimeout(8000);
                        
                        // 带上 Host 和移动端 UA
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("User-Agent", MOBILE_UA);
                        // 必须带上 Accept，否则 GitHub 可能返回乱码
                        conn.setRequestProperty("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
                        
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
                        // 异常放行
                    }
                }
                return super.shouldInterceptRequest(view, request);
            }

            // 处理非 HTTP(S) 协议（比如邮件、第三方 App 链接）
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url.startsWith("http://") || url.startsWith("https://")) {
                    return false; // 让 WebView 自己处理
                }
                try {
                    Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                    startActivity(intent);
                } catch (Exception e) {}
                return true;
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
