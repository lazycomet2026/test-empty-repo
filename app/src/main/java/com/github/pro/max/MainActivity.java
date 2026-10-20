package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.net.http.SslError;
import android.os.Bundle;
import android.view.Gravity;
import android.view.MotionEvent;
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
    private String currentIp = "140.82.112.4";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = getSharedPreferences("app_settings", MODE_PRIVATE);
        autoTranslate = prefs.getBoolean("auto_translate", false);
        currentIp = prefs.getString("custom_ip", "140.82.112.4");

        // 信任所有 SSL 证书
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

        FrameLayout rootLayout = new FrameLayout(this);
        rootLayout.setBackgroundColor(Color.parseColor("#0d1117"));

        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        // 使用真实的 Chrome 手机端 UA
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
                        String ipUrl = url.replace(host, currentIp);
                        
                        URL newUrl = new URL(ipUrl);
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setConnectTimeout(10000);
                        conn.setReadTimeout(10000);
                        conn.setInstanceFollowRedirects(false); 
                        
                        // 1. 100% 透传 WebView 的所有请求头，完美伪装成 Chrome
                        for (Map.Entry<String, String> entry : request.getRequestHeaders().entrySet()) {
                            String key = entry.getKey();
                            if (!key.equalsIgnoreCase("Host") && !key.equalsIgnoreCase("Accept-Encoding")) {
                                conn.setRequestProperty(key, entry.getValue());
                            }
                        }
                        // 强行修正 Host 和 Compression
                        conn.setRequestProperty("Host", host);
                        conn.setRequestProperty("Accept-Encoding", "identity"); // 获取明文，防止 WebView 二次解压错误
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) conn.setRequestProperty("Cookie", cookie);
                        
                        int code = conn.getResponseCode();
                        String reasonPhrase = conn.getResponseMessage();
                        
                        // 2. 100% 透传服务端的所有响应头，绝不过滤
                        Map<String, String> responseHeaders = new HashMap<>();
                        for (Map.Entry<String, List<String>> entry : conn.getHeaderFields().entrySet()) {
                            if (entry.getKey() != null && !entry.getValue().isEmpty()) {
                                String key = entry.getKey();
                                String val = entry.getValue().get(0);
                                if (key.equalsIgnoreCase("Location")) {
                                    val = val.replace(currentIp, host); // 重定向地址换回原域名
                                }
                                responseHeaders.put(key, val);
                            }
                        }
                        
                        // 3. 原封不动返回数据流
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
                        
                        return new WebResourceResponse(mime, enc, code, reasonPhrase, responseHeaders, is);
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }
        });

        rootLayout.addView(webView, new FrameLayout.LayoutParams(-1, -1));

        // 半透明悬浮球
        TextView transBtn = createFloatingBall("译");
        transBtn.setOnClickListener(v -> doTranslate());
        rootLayout.addView(transBtn);
        makeDraggable(transBtn);

        TextView setBtn = createFloatingBall("⚙");
        setBtn.setOnClickListener(v -> showSettingsDialog());
        rootLayout.addView(setBtn);
        makeDraggable(setBtn);

        webView.loadUrl("https://github.com");
        setContentView(rootLayout);
    }

    private TextView createFloatingBall(String text) {
        TextView tv = new TextView(this);
        tv.setText(text);
        tv.setTextColor(Color.WHITE);
        tv.setTextSize(18);
        tv.setGravity(Gravity.CENTER);
        
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#80000000")); // 50% 透明黑
        tv.setBackground(shape);
        
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(120, 120);
        params.gravity = Gravity.TOP | Gravity.START;
        if (text.equals("译")) params.setMargins(800, 300, 0, 0);
        else params.setMargins(800, 500, 0, 0);
        tv.setLayoutParams(params);
        return tv;
    }

    private void makeDraggable(final View view) {
        view.setOnTouchListener(new View.OnTouchListener() {
            private int initialX;
            private int initialY;
            private float initialTouchX;
            private float initialTouchY;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) v.getLayoutParams();
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = params.leftMargin;
                        initialY = params.topMargin;
                        initialTouchX = event.getRawX();
                        initialTouchY = event.getRawY();
                        return false;
                    case MotionEvent.ACTION_MOVE:
                        params.leftMargin = initialX + (int) (event.getRawX() - initialTouchX);
                        params.topMargin = initialY + (int) (event.getRawY() - initialTouchY);
                        v.setLayoutParams(params);
                        return true;
                }
                return false;
            }
        });
    }

    private void showSettingsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("设置");

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(50, 20, 50, 20);

        CheckBox autoTransCb = new CheckBox(this);
        autoTransCb.setText("自动翻译页面");
        autoTransCb.setChecked(autoTranslate);
        autoTransCb.setOnCheckedChangeListener((buttonView, isChecked) -> {
            autoTranslate = isChecked;
            prefs.edit().putBoolean("auto_translate", isChecked).apply();
        });
        layout.addView(autoTransCb);

        TextView ipLabel = new TextView(this);
        ipLabel.setText("自定义 IP:");
        layout.addView(ipLabel);

        EditText ipInput = new EditText(this);
        ipInput.setText(currentIp);
        layout.addView(ipInput);

        builder.setView(layout);
        builder.setPositiveButton("保存", (dialog, which) -> {
            String newIp = ipInput.getText().toString().trim();
            if (!newIp.isEmpty()) {
                currentIp = newIp;
                prefs.edit().putString("custom_ip", newIp).apply();
                webView.reload();
            }
        });
        builder.setNegativeButton("取消", null);
        builder.show();
    }

    private void doTranslate() {
        Toast.makeText(this, "正在翻译页面...", Toast.LENGTH_SHORT).show();
        String js = "javascript:(async function() {" +
                "const els = document.querySelectorAll('p, h1, h2, h3, h4, h5, li, span, a');" +
                "for (let el of els) {" +
                "  if (el.children.length > 0 || !el.innerText || el.innerText.trim().length < 2) continue;" +
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
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
