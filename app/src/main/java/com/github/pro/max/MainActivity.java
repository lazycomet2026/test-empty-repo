package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.net.http.SslError;
import android.os.Bundle;
import android.webkit.*;
import android.widget.Button;
import android.view.Gravity;
import android.view.View;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
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
    private boolean isTranslated = false;

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
        settings.setDatabaseEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
        settings.setSupportMultipleWindows(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        settings.setUserAgentString("Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36");

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        webView.setWebChromeClient(new WebChromeClient());

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed();
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                
                // 只拦截 GET 请求，防止登录卡死
                if (!"GET".equals(request.getMethod())) return super.shouldInterceptRequest(view, request);

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        String host = originalUrl.getHost();
                        String ip = ipList[currentIpIndex];
                        
                        // 替换为 IP
                        URL newUrl = new URL(originalUrl.getProtocol(), ip, originalUrl.getPort(), originalUrl.getFile());
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        // 缩短超时，绝不让拦截器卡死主进程
                        conn.setConnectTimeout(2000);
                        conn.setReadTimeout(2000);
                        conn.setInstanceFollowRedirects(false);
                        
                        // 完美透传请求头
                        for (Map.Entry<String, String> entry : request.getRequestHeaders().entrySet()) {
                            if (!entry.getKey().equalsIgnoreCase("Host") && !entry.getKey().equalsIgnoreCase("Accept-Encoding")) {
                                conn.setRequestProperty(entry.getKey(), entry.getValue());
                            }
                        }
                        conn.setRequestProperty("Host", host);
                        conn.setRequestProperty("Accept-Encoding", "identity"); // 防乱码
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) conn.setRequestProperty("Cookie", cookie);
                        
                        int code = conn.getResponseCode();
                        
                        // 完美处理重定向
                        if (code == 301 || code == 302 || code == 307 || code == 308) {
                            Map<String, String> responseHeaders = new HashMap<>();
                            String location = conn.getHeaderField("Location");
                            if (location != null) {
                                if (location.contains(ip)) {
                                    location = location.replace(ip, "github.com");
                                }
                                responseHeaders.put("Location", location);
                            }
                            return new WebResourceResponse("text/html", "utf-8", code, conn.getResponseMessage(), responseHeaders, null);
                        }
                        
                        // 完美接管 Content-Type
                        InputStream is = conn.getInputStream();
                        String contentType = conn.getContentType();
                        if (contentType != null) {
                            String[] parts = contentType.split(";");
                            String mime = parts[0].trim();
                            String enc = (parts.length > 1 && parts[1].contains("charset=")) ? parts[1].split("charset=")[1].trim() : "utf-8";
                            return new WebResourceResponse(mime, enc, is);
                        }
                        return new WebResourceResponse("text/html", "utf-8", is);
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (request.isForMainFrame()) {
                    if (currentIpIndex < ipList.length - 1) {
                        currentIpIndex++;
                        view.stopLoading();
                        view.loadUrl("https://github.com");
                    } else {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("连接超时")
                                .setMessage("当前所有节点均不可用，请检查网络。")
                                .setPositiveButton("重试", (dialog, which) -> {
                                    currentIpIndex = 0;
                                    view.stopLoading();
                                    view.loadUrl("https://github.com");
                                })
                                .setNegativeButton("退出", (dialog, which) -> finish())
                                .show();
                    }
                }
            }
        });
        
        // 添加悬浮翻译按钮
        Button transBtn = new Button(this);
        transBtn.setText("译");
        transBtn.setTextColor(Color.WHITE);
        transBtn.setTextSize(16);
        transBtn.setPadding(0, 0, 0, 0);
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#238636"));
        transBtn.setBackground(shape);
        
        android.widget.FrameLayout root = new android.widget.FrameLayout(this);
        android.widget.FrameLayout.LayoutParams params = new android.widget.FrameLayout.LayoutParams(120, 120);
        params.gravity = Gravity.BOTTOM | Gravity.END;
        params.setMargins(0, 0, 40, 120);
        root.addView(webView, new android.widget.FrameLayout.LayoutParams(-1, -1));
        root.addView(transBtn, params);
        
        transBtn.setOnClickListener(v -> {
            if (!isTranslated) {
                injectTranslator();
                transBtn.setText("原");
                isTranslated = true;
            } else {
                webView.reload();
                transBtn.setText("译");
                isTranslated = false;
            }
        });
        
        webView.loadUrl("https://github.com");
        setContentView(root);
    }

    private void injectTranslator() {
        String js = "javascript:(async function() {" +
                "const texts = document.querySelectorAll('p, h1, h2, h3, h4, h5, li, span, a');" +
                "for (let el of texts) {" +
                "  if (el.children.length > 0) continue;" +
                "  let txt = el.innerText;" +
                "  if (!txt || txt.trim() === '') continue;" +
                "  let url = 'https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=zh-CN&dt=t&q=' + encodeURIComponent(txt);" +
                "  try {" +
                "    let res = await fetch(url);" +
                "    let data = await res.json();" +
                "    if(data && data[0] && data[0][0] && data[0][0][0]) {" +
                "      el.innerText = data[0][0][0];" +
                "    }" +
                "  } catch(e) {}" +
                "}" +
                "})()";
        webView.evaluateJavascript(js, null);
    }

    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            webView.stopLoading();
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
