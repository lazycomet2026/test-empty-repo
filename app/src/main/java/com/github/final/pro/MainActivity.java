package com.github.final.pro;

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
        
        // 完美 Chrome 伪装
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

                if (url.contains("github.com") || url.contains("githubassets.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), currentIp, originalUrl.getPort(), originalUrl.getFile());
                        HttpsURLConnection conn = (HttpsURLConnection) newUrl.openConnection();
                        
                        conn.setConnectTimeout(8000);
                        conn.setReadTimeout(8000);
                        conn.setInstanceFollowRedirects(false); // 重定向交给 WebView
                        
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("Accept-Encoding", "identity"); 
                        
                        // 只要 MIME 和流，绝对不传任何会导致渲染错乱的头
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
                        
                        return new WebResourceResponse(mime, enc, conn.getInputStream());
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }
        });

        rootLayout.addView(webView, new FrameLayout.LayoutParams(-1, -1));

        // 悬浮翻译（全透明，可拖动）
        TextView transBtn = createFloatingBall("译");
        transBtn.setOnClickListener(v -> doTranslate());
        rootLayout.addView(transBtn);
        makeDraggable(transBtn);

        // 悬浮设置（全透明，可拖动）
        TextView setBtn = createFloatingBall("⚙");
        setBtn.setOnClickListener(v -> showSettingsDialog());
        rootLayout.addView(setBtn);
        makeDraggable(setBtn);

        webView.loadUrl("https://github.com");
        setContentView(rootLayout);
    }

    // 【关键修改】完全透明的悬浮球
    private TextView createFloatingBall(String text) {
        TextView tv = new TextView(this);
        tv.setText(text);
        tv.setTextColor(Color.WHITE);
        tv.setTextSize(24); // 字体大一点，因为没有背景圈了
        tv.setGravity(Gravity.CENTER);
        
        // 无背景，或者使用完全透明的背景
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#00000000")); // 全透明
        tv.setBackground(shape);
        
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(150, 150);
        params.gravity = Gravity.TOP | Gravity.START;
        if (text.equals("译")) params.setMargins(700, 300, 0, 0);
        else params.setMargins(700, 500, 0, 0);
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
