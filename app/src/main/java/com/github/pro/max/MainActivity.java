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
import java.util.Map;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import java.security.cert.X509Certificate;
import java.security.SecureRandom;

public class MainActivity extends Activity {
    private WebView webView;
    
    // 优质 IP 池
    private String[] ipList = {
        "140.82.112.4", "140.82.113.4", "140.82.114.4",
        "140.82.116.3", "20.205.243.166", "20.205.243.168"
    };
    private int currentIpIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // 【核心修复1】全局信任所有 SSL 证书，解决直连 IP 时的证书错误，修复 UI 错乱
        try {
            TrustManager[] trustAllCerts = new TrustManager[]{
                new X509TrustManager() {
                    public X509Certificate[] getAcceptedIssuers() { return new X509Certificate[0]; }
                    public void checkClientTrusted(X509Certificate[] certs, String authType) {}
                    public void checkServerTrusted(X509Certificate[] certs, String authType) {}
                }
            };
            SSLContext sc = SSLContext.getInstance("TLS");
            sc.init(null, trustAllCerts, new SecureRandom());
            HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
            HttpsURLConnection.setDefaultHostnameVerifier((hostname, session) -> true);
        } catch (Exception e) {
            e.printStackTrace();
        }

        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 伪装成手机端 Chrome，让 GitHub 下发完美的手机 UI
        settings.setUserAgentString("Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36");
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);

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
                if (!"GET".equals(request.getMethod())) {
                    return super.shouldInterceptRequest(view, request);
                }

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        String host = originalUrl.getHost();
                        URL newUrl = new URL(originalUrl.getProtocol(), ipList[currentIpIndex], originalUrl.getPort(), originalUrl.getFile());
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setConnectTimeout(8000);
                        conn.setReadTimeout(8000);
                        
                        // 【核心修复2】透传所有请求头，防止 CSS/JS 加载错乱
                        for (Map.Entry<String, String> header : request.getRequestHeaders().entrySet()) {
                            if (!header.getKey().equalsIgnoreCase("Host")) {
                                conn.setRequestProperty(header.getKey(), header.getValue());
                            }
                        }
                        // 强制覆盖 Host 为真实的 github 域名
                        conn.setRequestProperty("Host", host);
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) {
                            conn.setRequestProperty("Cookie", cookie);
                        }
                        
                        InputStream is = conn.getInputStream();
                        String contentType = conn.getContentType();
                        String encoding = conn.getContentEncoding();
                        if (contentType == null) contentType = "text/html";
                        if (encoding == null) encoding = "utf-8";
                        
                        // 返回完整数据给 WebView
                        return new WebResourceResponse(contentType, encoding, is);
                    } catch (Exception e) {
                        // 失败就放行，防止死锁
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
