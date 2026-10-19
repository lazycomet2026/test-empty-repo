package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.net.http.SslError;
import android.os.Bundle;
import android.webkit.*;
import java.io.InputStream;
import java.net.URL;
import java.util.Map;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import java.security.cert.X509Certificate;
import java.security.SecureRandom;

public class MainActivity extends Activity {
    private WebView webView;
    private String[] ipList = {
        "140.82.112.4", "140.82.113.4", "140.82.114.4",
        "140.82.116.3", "20.205.243.166", "20.205.243.168"
    };
    private int currentIpIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // 全局信任 SSL 证书，防止 IP 直连报错白屏
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
        settings.setDatabaseEnabled(true);
        settings.setUserAgentString("Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36");
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        settings.setSupportMultipleWindows(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        // 加上 WebChromeClient，处理弹窗和 JS 交互
        webView.setWebChromeClient(new WebChromeClient());

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed(); // 强制忽略证书错误
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                
                // 放行 POST 等请求，绝对不能碰
                if (!"GET".equals(request.getMethod())) {
                    return super.shouldInterceptRequest(view, request);
                }

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        String host = originalUrl.getHost();
                        String ip = ipList[currentIpIndex];
                        
                        // 构造真实的底层请求地址
                        URL newUrl = new URL(originalUrl.getProtocol(), ip, originalUrl.getPort(), originalUrl.getFile());
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setConnectTimeout(10000);
                        conn.setReadTimeout(10000);
                        
                        // 完美透传请求头
                        Map<String, String> reqHeaders = request.getRequestHeaders();
                        for (Map.Entry<String, String> entry : reqHeaders.entrySet()) {
                            String key = entry.getKey();
                            if (!key.equalsIgnoreCase("Host") && !key.equalsIgnoreCase("Accept-Encoding")) {
                                conn.setRequestProperty(key, entry.getValue());
                            }
                        }
                        conn.setRequestProperty("Host", host);
                        conn.setRequestProperty("Accept-Encoding", "identity"); // 强制无压缩防乱码
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) {
                            conn.setRequestProperty("Cookie", cookie);
                        }
                        
                        InputStream is = conn.getInputStream();
                        
                        // 【终极修复】精确解析 MIME 类型和编码，这是解决"源码"和"错位"的绝对核心
                        String contentType = conn.getContentType();
                        String mimeType = "text/html";
                        String encoding = "utf-8";
                        
                        if (contentType != null) {
                            String[] parts = contentType.split(";");
                            mimeType = parts[0].trim();
                            if (parts.length > 1) {
                                String charset = parts[1].trim();
                                if (charset.startsWith("charset=")) {
                                    encoding = charset.substring(8).trim();
                                }
                            }
                        } else {
                            // 如果没拿到 Content-Type，根据后缀兜底
                            if (url.endsWith(".css")) mimeType = "text/css";
                            else if (url.endsWith(".js")) mimeType = "application/javascript";
                            else if (url.endsWith(".png")) mimeType = "image/png";
                            else if (url.endsWith(".jpg") || url.endsWith(".jpeg")) mimeType = "image/jpeg";
                            else if (url.endsWith(".svg")) mimeType = "image/svg+xml";
                            else if (url.endsWith(".woff2")) mimeType = "font/woff2";
                            else if (url.endsWith(".ico")) mimeType = "image/x-icon";
                        }
                        
                        // 返回完美识别类型的流
                        return new WebResourceResponse(mimeType, encoding, is);
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
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("连接超时")
                                .setMessage("当前所有节点均不可用，请检查网络。")
                                .setPositiveButton("重试", (dialog, which) -> {
                                    currentIpIndex = 0;
                                    webView.loadUrl("https://github.com");
                                })
                                .setNegativeButton("退出", (dialog, which) -> finish())
                                .show();
                    }
                }
            }
        });
        
        // 用标准域名加载，彻底交给 WebView 渲染
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
