package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.http.SslError;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
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
    private EditText urlInput;
    private LinearLayout topBar, bottomBar;
    private SharedPreferences prefs;
    private boolean isDarkMode = true;
    private boolean autoTranslate = false;
    private boolean translatorEnabled = true;

    private String[] ipList = {"140.82.112.4", "140.82.113.4", "140.82.114.4", "140.82.116.3", "20.205.243.166"};
    private int currentIpIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = getSharedPreferences("app_settings", MODE_PRIVATE);
        isDarkMode = prefs.getBoolean("dark_mode", true);
        autoTranslate = prefs.getBoolean("auto_translate", false);
        translatorEnabled = prefs.getBoolean("translator_enabled", true);

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

        LinearLayout rootLayout = new LinearLayout(this);
        rootLayout.setOrientation(LinearLayout.VERTICAL);
        applyTheme(rootLayout);

        // --- 顶部工具栏 ---
        topBar = new LinearLayout(this);
        topBar.setOrientation(LinearLayout.HORIZONTAL);
        topBar.setPadding(20, 20, 20, 20);
        topBar.setGravity(Gravity.CENTER_VERTICAL);

        Button backBtn = createButton("←");
        backBtn.setOnClickListener(v -> { if (webView.canGoBack()) webView.goBack(); });
        topBar.addView(backBtn);

        Button refreshBtn = createButton("↻");
        refreshBtn.setOnClickListener(v -> webView.reload());
        topBar.addView(refreshBtn);

        Button settingsBtn = createButton("⚙");
        settingsBtn.setOnClickListener(v -> showSettingsDialog());
        topBar.addView(settingsBtn);

        Button transBtn = createButton("译");
        transBtn.setOnClickListener(v -> doTranslate());
        topBar.addView(transBtn);

        rootLayout.addView(topBar, new LinearLayout.LayoutParams(-1, -2));

        // --- 核心 WebView ---
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
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) { handler.proceed(); }

            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                if (autoTranslate && translatorEnabled) {
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
                        
                        for (Map.Entry<String, String> entry : request.getRequestHeaders().entrySet()) {
                            if (!entry.getKey().equalsIgnoreCase("Host") && !entry.getKey().equalsIgnoreCase("Accept-Encoding")) {
                                conn.setRequestProperty(entry.getKey(), entry.getValue());
                            }
                        }
                        conn.setRequestProperty("Host", host);
                        conn.setRequestProperty("Accept-Encoding", "identity"); // 强制无压缩
                        
                        String cookie = CookieManager.getInstance().getCookie(url);
                        if (cookie != null) conn.setRequestProperty("Cookie", cookie);
                        
                        int code = conn.getResponseCode();
                        Map<String, String> responseHeaders = new HashMap<>();
                        
                        // 完整透传所有响应头，彻底解决排版错乱
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

        rootLayout.addView(webView, new LinearLayout.LayoutParams(-1, 0, 1.0f));

        // --- 底部地址栏 ---
        bottomBar = new LinearLayout(this);
        bottomBar.setOrientation(LinearLayout.HORIZONTAL);
        bottomBar.setPadding(20, 10, 20, 10);
        
        urlInput = new EditText(this);
        urlInput.setHint("输入网址...");
        urlInput.setText("https://github.com");
        bottomBar.addView(urlInput, new LinearLayout.LayoutParams(0, -2, 1.0f));

        Button goBtn = createButton("前往");
        goBtn.setOnClickListener(v -> {
            String u = urlInput.getText().toString();
            if (!u.startsWith("http")) u = "https://" + u;
            webView.loadUrl(u);
        });
        bottomBar.addView(goBtn);

        rootLayout.addView(bottomBar, new LinearLayout.LayoutParams(-1, -2));

        applyThemeColors(); // 刷新配色
        webView.loadUrl("https://github.com");
        setContentView(rootLayout);
    }

    // 原生设置弹窗
    private void showSettingsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("设置");

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(40, 20, 40, 20);

        CheckBox autoTransCb = new CheckBox(this);
        autoTransCb.setText("自动翻译页面");
        autoTransCb.setChecked(autoTranslate);
        autoTransCb.setOnCheckedChangeListener((buttonView, isChecked) -> {
            autoTranslate = isChecked;
            prefs.edit().putBoolean("auto_translate", isChecked).apply();
        });
        layout.addView(autoTransCb);

        CheckBox transEnableCb = new CheckBox(this);
        transEnableCb.setText("启用翻译功能");
        transEnableCb.setChecked(translatorEnabled);
        transEnableCb.setOnCheckedChangeListener((buttonView, isChecked) -> {
            translatorEnabled = isChecked;
            prefs.edit().putBoolean("translator_enabled", isChecked).apply();
        });
        layout.addView(transEnableCb);

        CheckBox darkModeCb = new CheckBox(this);
        darkModeCb.setText("深色模式");
        darkModeCb.setChecked(isDarkMode);
        darkModeCb.setOnCheckedChangeListener((buttonView, isChecked) -> {
            isDarkMode = isChecked;
            prefs.edit().putBoolean("dark_mode", isChecked).apply();
            applyThemeColors();
        });
        layout.addView(darkModeCb);

        builder.setView(layout);
        builder.setPositiveButton("确定", null);
        builder.show();
    }

    // 原生按钮生成
    private Button createButton(String text) {
        Button btn = new Button(this);
        btn.setText(text);
        btn.setTextSize(16);
        return btn;
    }

    // 自动翻译逻辑
    private void doTranslate() {
        if (!translatorEnabled) return;
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

    // 配色主题引擎
    private void applyThemeColors() {
        int bgColor = isDarkMode ? Color.parseColor("#0d1117") : Color.parseColor("#ffffff");
        int barColor = isDarkMode ? Color.parseColor("#161b22") : Color.parseColor("#f6f8fa");
        int textColor = isDarkMode ? Color.parseColor("#c9d1d9") : Color.parseColor("#1f2328");
        int accentColor = Color.parseColor("#238636");

        topBar.setBackgroundColor(barColor);
        bottomBar.setBackgroundColor(barColor);
        urlInput.setTextColor(textColor);
        urlInput.setHintTextColor(isDarkMode ? Color.GRAY : Color.LTGRAY);

        for (int i = 0; i < topBar.getChildCount(); i++) {
            View v = topBar.getChildAt(i);
            if (v instanceof Button) {
                v.setBackgroundColor(accentColor);
                ((Button) v).setTextColor(Color.WHITE);
            }
        }

        View goBtn = bottomBar.getChildAt(1);
        if (goBtn instanceof Button) {
            goBtn.setBackgroundColor(accentColor);
            ((Button) goBtn).setTextColor(Color.WHITE);
        }
    }

    private void applyTheme(LinearLayout layout) {
        layout.setBackgroundColor(isDarkMode ? Color.parseColor("#0d1117") : Color.parseColor("#ffffff"));
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
