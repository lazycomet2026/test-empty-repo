package com.github.final.app;

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
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

public class MainActivity extends Activity {
    private WebView webView;
    private SharedPreferences prefs;
    private boolean autoTranslate = false;
    private String[] ipList = {"140.82.112.4", "140.82.113.4", "140.82.114.4", "20.205.243.166"};
    private int currentIpIndex = 0;
    private OkHttpClient httpClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = getSharedPreferences("app_settings", MODE_PRIVATE);
        autoTranslate = prefs.getBoolean("auto_translate", false);
        currentIpIndex = prefs.getInt("ip_index", 0);

        httpClient = new OkHttpClient.Builder()
                .connectTimeout(10, TimeUnit.SECONDS)
                .readTimeout(10, TimeUnit.SECONDS)
                .followRedirects(false)
                .sslSocketFactory(getTrustAllSslSocketFactory(), getTrustAllTrustManager())
                .hostnameVerifier((hostname, session) -> true)
                .build();

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
                if (autoTranslate) doTranslate();
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                if (!"GET".equals(request.getMethod())) return super.shouldInterceptRequest(view, request);

                if (url.contains("github.com") || url.contains("githubassets.com")) {
                    try {
                        String host = new java.net.URL(url).getHost();
                        String targetUrl = url.replace(host, ipList[currentIpIndex]);
                        
                        Request.Builder reqBuilder = new Request.Builder().url(targetUrl).get();
                        for (Map.Entry<String, String> entry : request.getRequestHeaders().entrySet()) {
                            if (!entry.getKey().equalsIgnoreCase("Host") && !entry.getKey().equalsIgnoreCase("Connection")) {
                                reqBuilder.addHeader(entry.getKey(), entry.getValue());
                            }
                        }
                        reqBuilder.addHeader("Host", host);
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) reqBuilder.addHeader("Cookie", cookie);

                        Response response = httpClient.newCall(reqBuilder.build()).execute();
                        ResponseBody body = response.body();
                        InputStream is = body != null ? body.byteStream() : null;
                        if (is == null) return super.shouldInterceptRequest(view, request);

                        String contentType = response.header("Content-Type");
                        String mime = "text/html";
                        String enc = "utf-8";
                        if (contentType != null) {
                            String[] parts = contentType.split(";");
                            mime = parts[0].trim();
                            if (parts.length > 1 && parts[1].contains("charset=")) {
                                enc = parts[1].split("charset=")[1].trim();
                            }
                        }

                        Map<String, String> responseHeaders = new HashMap<>();
                        for (String name : response.headers().names()) {
                            String val = response.header(name);
                            if (name.equalsIgnoreCase("Location")) {
                                val = val.replace(ipList[currentIpIndex], host);
                            }
                            responseHeaders.put(name, val);
                        }

                        return new WebResourceResponse(mime, enc, response.code(), response.message(), responseHeaders, is);
                    } catch (Exception e) {
                        currentIpIndex = (currentIpIndex + 1) % ipList.length;
                        prefs.edit().putInt("ip_index", currentIpIndex).apply();
                    }
                }
                return super.shouldInterceptRequest(view, request);
            }
        });

        rootLayout.addView(webView, new FrameLayout.LayoutParams(-1, -1));

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

    private javax.net.ssl.SSLSocketFactory getTrustAllSslSocketFactory() {
        try {
            javax.net.ssl.SSLContext sc = javax.net.ssl.SSLContext.getInstance("TLS");
            sc.init(null, new javax.net.ssl.TrustManager[]{getTrustAllTrustManager()}, new java.security.SecureRandom());
            return sc.getSocketFactory();
        } catch (Exception e) { return null; }
    }
    private javax.net.ssl.X509TrustManager getTrustAllTrustManager() {
        return new javax.net.ssl.X509TrustManager() {
            public void checkClientTrusted(java.security.cert.X509Certificate[] chain, String authType) {}
            public void checkServerTrusted(java.security.cert.X509Certificate[] chain, String authType) {}
            public java.security.cert.X509Certificate[] getAcceptedIssuers() { return new java.security.cert.X509Certificate[0]; }
        };
    }

    private TextView createFloatingBall(String text) {
        TextView tv = new TextView(this);
        tv.setText(text);
        tv.setTextColor(Color.WHITE);
        tv.setTextSize(20);
        tv.setGravity(Gravity.CENTER);
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#66000000")); 
        tv.setBackground(shape);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(140, 140);
        params.gravity = Gravity.TOP | Gravity.START;
        if (text.equals("译")) params.setMargins(750, 300, 0, 0);
        else params.setMargins(750, 500, 0, 0);
        tv.setLayoutParams(params);
        return tv;
    }

    private void makeDraggable(final View view) {
        view.setOnTouchListener(new View.OnTouchListener() {
            private int initialX, initialY;
            private float initialTouchX, initialTouchY;
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
        ipLabel.setText("当前节点: " + ipList[currentIpIndex]);
        layout.addView(ipLabel);

        Button resetIpBtn = new Button(this);
        resetIpBtn.setText("重置节点并重新测速");
        resetIpBtn.setOnClickListener(v -> {
            currentIpIndex = 0;
            prefs.edit().putInt("ip_index", 0).apply();
            webView.reload();
        });
        layout.addView(resetIpBtn);

        builder.setView(layout);
        builder.setPositiveButton("保存", null);
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
        if (webView != null && webView.canGoBack()) webView.goBack();
        else super.onBackPressed();
    }
}
