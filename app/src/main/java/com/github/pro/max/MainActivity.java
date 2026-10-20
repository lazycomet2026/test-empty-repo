package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.net.http.SslError;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.webkit.*;
import android.widget.*;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import java.security.cert.X509Certificate;
import java.security.SecureRandom;

public class MainActivity extends Activity {
    private WebView webView;
    private SharedPreferences prefs;
    private boolean autoTranslate = false;

    private String[] ipList = {"140.82.112.4", "140.82.113.4", "140.82.114.4", "20.205.243.166"};
    private int currentIpIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = getSharedPreferences("app_settings", MODE_PRIVATE);
        autoTranslate = prefs.getBoolean("auto_translate", false);

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

        // 纯全屏 FrameLayout
        FrameLayout rootLayout = new FrameLayout(this);
        rootLayout.setBackgroundColor(Color.parseColor("#0d1117"));

        // 全屏 WebView
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
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
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                if (autoTranslate) {
                    doTranslate();
                }
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                if (!"GET".equals(request.getMethod())) return super.shouldInterceptRequest(view, request);

                if (url.contains("github.com") || url.contains("githubassets.com") || url.contains("githubusercontent.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        String host = originalUrl.getHost();
                        String ip = ipList[currentIpIndex];
                        
                        URL newUrl = new URL(originalUrl.getProtocol(), ip, originalUrl.getPort(), originalUrl.getFile());
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setConnectTimeout(4000);
                        conn.setReadTimeout(4000);
                        conn.setInstanceFollowRedirects(false);
                        
                        // 完美透传所有原始请求头
                        for (Map.Entry<String, String> entry : request.getRequestHeaders().entrySet()) {
                            if (!entry.getKey().equalsIgnoreCase("Host") && !entry.getKey().equalsIgnoreCase("Accept-Encoding")) {
                                conn.setRequestProperty(entry.getKey(), entry.getValue());
                            }
                        }
                        conn.setRequestProperty("Host", host);
                        conn.setRequestProperty("Accept-Encoding", "identity");
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) conn.setRequestProperty("Cookie", cookie);
                        
                        int code = conn.getResponseCode();
                        Map<String, String> responseHeaders = new HashMap<>();
                        
                        // 完美透传响应头（彻底解决主页错位）
                        for (Map.Entry<String, List<String>> entry : conn.getHeaderFields().entrySet()) {
                            if (entry.getKey() != null && !entry.getValue().isEmpty()) {
                                String key = entry.getKey();
                                String val = entry.getValue().get(0);
                                if (key.equalsIgnoreCase("Location") && val.contains(ip)) {
                                    val = val.replace(ip, "github.com");
                                }
                                if (!key.equalsIgnoreCase("Content-Encoding") && !key.equalsIgnoreCase("Transfer-Encoding")) {
                                    responseHeaders.put(key, val);
                                }
                            }
                        }
                        
                        InputStream is = (code >= 400) ? conn.getErrorStream() : conn.getInputStream();
                        if (is == null) return super.shouldInterceptRequest(view, request);
                        
                        String contentType = conn.getContentType();
                        String mime = "text/html";
                        String enc = "utf-8";
                        if (contentType != null) {
                            String[] parts = contentType.split(";");
                            mime = parts[0].trim();
                            if (parts.length > 1 && parts[1].contains("charset=")) {
                                enc = parts[1].split("charset=")[1].trim();
                            }
                        }
                        return new WebResourceResponse(mime, enc, code, conn.getResponseMessage(), responseHeaders, is);
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (request.isForMainFrame() && currentIpIndex < ipList.length - 1) {
                    currentIpIndex++;
                    view.stopLoading();
                    view.loadUrl("https://github.com");
                }
            }
        });

        rootLayout.addView(webView, new FrameLayout.LayoutParams(-1, -1));

        // 悬浮的“翻译”按钮
        Button transBtn = createFloatingButton("译");
        FrameLayout.LayoutParams transParams = new FrameLayout.LayoutParams(120, 120);
        transParams.gravity = Gravity.BOTTOM | Gravity.END;
        transParams.setMargins(0, 0, 30, 200);
        transBtn.setOnClickListener(v -> doTranslate());
        rootLayout.addView(transBtn, transParams);

        // 悬浮的“设置”按钮
        Button setBtn = createFloatingButton("⚙");
        FrameLayout.LayoutParams setParams = new FrameLayout.LayoutParams(120, 120);
        setParams.gravity = Gravity.BOTTOM | Gravity.END;
        setParams.setMargins(0, 0, 30, 350);
        setBtn.setOnClickListener(v -> showSettingsDialog());
        rootLayout.addView(setBtn, setParams);

        webView.loadUrl("https://github.com");
        setContentView(rootLayout);
    }

    private Button createFloatingButton(String text) {
        Button btn = new Button(this);
        btn.setText(text);
        btn.setTextColor(Color.WHITE);
        btn.setTextSize(16);
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#238636"));
        btn.setBackground(shape);
        return btn;
    }

    private void showSettingsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("设置");

        CheckBox autoTransCb = new CheckBox(this);
        autoTransCb.setText("自动翻译页面");
        autoTransCb.setChecked(autoTranslate);
        autoTransCb.setOnCheckedChangeListener((buttonView, isChecked) -> {
            autoTranslate = isChecked;
            prefs.edit().putBoolean("auto_translate", isChecked).apply();
        });

        builder.setView(autoTransCb);
        builder.setPositiveButton("确定", null);
        builder.show();
    }

    private void doTranslate() {
        Toast.makeText(this, "正在翻译页面...", Toast.LENGTH_SHORT).show();
        String js = "javascript:(async function() {" +
                "const els = document.querySelectorAll('p, h1, h2, h3, h4, h5, li, span, a, div');" +
                "for (let el of els) {" +
                "  if (el.children.length > 0 || !el.innerText || el.innerText.trim().length < 3) continue;" +
                "  let txt = el.innerText.trim();" +
                "  try {" +
                "    let res = await fetch('https://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=zh-CN&dt=t&q=' + encodeURIComponent(txt));" +
                "    let data = await res.json();" +
                "    if(data && data[0] && data[0][0] && data[0][0][0]) { el.innerText = data[0][0][0]; }" +
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
