package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.webkit.*;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity {
    private WebView webView;
    // 内置几个备用 IP，一旦某个卡死，自动测速切换
    private String[] ipList = {"140.82.114.3", "20.205.243.166", "140.82.112.4", "140.82.113.4"};
    private int currentIpIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 强制支持混合内容和跨域
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        
        // 开启 Cookie 持久化，保证记住登录状态
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                try {
                    if (request.getUrl() == null) return super.shouldInterceptRequest(view, request);
                    String url = request.getUrl().toString();
                    
                    // 核心逻辑：拦截所有发往 github.com 和 githubassets.com 的请求，强制走 IP
                    if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), ipList[currentIpIndex], originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("User-Agent", view.getSettings().getUserAgentString());
                        
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
                    }
                } catch (Exception e) {
                    // 发生异常时，放行请求（比如某些图片加载失败）
                }
                return super.shouldInterceptRequest(view, request);
            }

            // 处理超时错误
            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                // 如果主页面加载失败（超时）
                if (request.isForMainFrame()) {
                    if (currentIpIndex < ipList.length - 1) {
                        // 尝试切换到下一个 IP
                        currentIpIndex++;
                        webView.loadUrl("https://github.com");
                    } else {
                        // 所有 IP 都试过了，弹窗提示
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                new AlertDialog.Builder(MainActivity.this)
                                        .setTitle("连接超时")
                                        .setMessage("当前所有节点均不可用，请检查网络或稍后再试。")
                                        .setPositiveButton("重试", (dialog, which) -> {
                                            currentIpIndex = 0;
                                            webView.loadUrl("https://github.com");
                                        })
                                        .setNegativeButton("退出", (dialog, which) -> finish())
                                        .show();
                            }
                        });
                    }
                }
            }
        });
        
        // 直接加载原生 GitHub，不加任何 iframe 套壳
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
