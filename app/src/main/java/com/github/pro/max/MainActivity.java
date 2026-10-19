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
import java.io.ByteArrayInputStream;
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
        
        // 全局信任 SSL 证书
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
        } catch (Exception e) {}

        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        
        // 必须伪装成手机端，否则 GitHub 下发电脑版就会排版错乱
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
                // 只拦截主页面，其他的资源让 WebView 自己用系统网络加载
                if (request.isForMainFrame() && url.contains("github.com")) {
                    try {
                        String currentIp = ipList[currentIpIndex];
                        URL newUrl = new URL("https://" + currentIp + "/");
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setRequestProperty("Host", "github.com");
                        conn.setRequestProperty("User-Agent", "Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36");
                        conn.setRequestProperty("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
                        conn.setRequestProperty("Accept-Encoding", "identity"); // 绝对不压缩，否则乱码
                        
                        InputStream is = conn.getInputStream();
                        
                        // 读取出 HTML 源码
                        java.util.Scanner s = new java.util.Scanner(is).useDelimiter("\\A");
                        String html = s.hasNext() ? s.next() : "";
                        
                        // 【核心黑科技】在 HTML 里强行注入一段 JS，把所有的 github.com 替换成我们的 IP
                        String js = "<script>" +
                            "var ip = '" + currentIp + "';" +
                            "var html = document.documentElement.innerHTML;" +
                            "html = html.split('github.com').join(ip);" +
                            "html = html.split('githubassets.com').join(ip);" +
                            "html = html.split('githubusercontent.com').join(ip);" +
                            "document.documentElement.innerHTML = html;" +
                            "</script>";
                        
                        if (html.contains("</head>")) {
                            html = html.replace("</head>", js + "</head>");
                        } else {
                            html += js;
                        }
                        
                        return new WebResourceResponse("text/html", "utf-8", new ByteArrayInputStream(html.getBytes("utf-8")));
                    } catch (Exception e) {
                        return null; // 失败就放行，不显示代码
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
