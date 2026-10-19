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
    
    // 【核心更新】扩充为最新的国内实测高可用 IP 池（包含多个网段）
    private String[] ipList = {
        "140.82.112.4", 
        "140.82.113.4", 
        "140.82.114.4", 
        "140.82.116.3", 
        "20.205.243.166", 
        "20.205.243.168",
        "140.82.114.3",
        "140.82.112.3"
    };
    private int currentIpIndex = 0;

    // 伪装成移动端 Chrome，获取 GitHub 官方最适配的手机 UI
    private static final String MOBILE_UA = "Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        settings.setUserAgentString(MOBILE_UA);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        settings.setSupportMultipleWindows(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onCreateWindow(WebView view, boolean isDialog, boolean isUserGesture, android.os.Message resultMsg) {
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
                
                // 放行 POST 等非 GET 请求，避免登录或表单提交死锁
                if (!"GET".equals(request.getMethod())) {
                    return super.shouldInterceptRequest(view, request);
                }

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), ipList[currentIpIndex], originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        
                        // 设置合理的连接超时，防止单节点卡死
                        conn.setConnectTimeout(6000);
                        conn.setReadTimeout(6000);
                        
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("User-Agent", MOBILE_UA);
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
                        // 连接失败放行，交给主框架错误回调处理
                    }
                }
                return super.shouldInterceptRequest(view, request);
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url.startsWith("http://") || url.startsWith("https://")) {
                    return false;
                }
                try {
                    Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                    startActivity(intent);
                } catch (Exception e) {}
                return true;
            }

            // 【核心修复】主框架超时错误时，自动轮询下一个 IP
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
