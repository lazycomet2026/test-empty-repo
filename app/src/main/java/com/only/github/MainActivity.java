package com.only.github;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.webkit.*;
import android.widget.Button;
import android.view.Gravity;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;

import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends Activity {
    private static final String CUSTOM_IP = "20.205.243.166"; 
    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // 使用 FrameLayout 以便把按钮悬浮在上面
        android.widget.FrameLayout root = new android.widget.FrameLayout(this);
        root.setBackgroundColor(Color.parseColor("#0d1117"));

        webView = new WebView(this);
        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        // 允许混合内容（方便国内加载）
        webView.getSettings().setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                if (url.contains("github.com")) {
                    try {
                        URL originalUrl = new URL(url);
                        URL newUrl = new URL(originalUrl.getProtocol(), CUSTOM_IP, originalUrl.getPort(), originalUrl.getFile());
                        URLConnection conn = newUrl.openConnection();
                        conn.setRequestProperty("Host", originalUrl.getHost());
                        conn.setRequestProperty("User-Agent", view.getSettings().getUserAgentString());
                        InputStream is = conn.getInputStream();
                        String contentType = conn.getContentType();
                        String encoding = conn.getContentEncoding();
                        if (contentType == null) contentType = "text/html";
                        if (encoding == null) encoding = "utf-8";
                        return new WebResourceResponse(contentType, encoding, is);
                    } catch (Exception e) {}
                }
                return super.shouldInterceptRequest(view, request);
            }
        });

        webView.loadUrl("https://github.com");

        // 创建一个悬浮的“译”按钮
        Button transBtn = new Button(this);
        transBtn.setText("译");
        transBtn.setTextColor(Color.WHITE);
        transBtn.setTextSize(16);
        transBtn.setPadding(20, 10, 20, 10);
        
        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.OVAL);
        shape.setColor(Color.parseColor("#fb8500")); // 橙色显眼
        transBtn.setBackground(shape);

        android.widget.FrameLayout.LayoutParams params = new android.widget.FrameLayout.LayoutParams(
                android.widget.FrameLayout.LayoutParams.WRAP_CONTENT,
                android.widget.FrameLayout.LayoutParams.WRAP_CONTENT
        );
        params.gravity = Gravity.BOTTOM | Gravity.END;
        params.setMargins(0, 0, 50, 100); // 放在右下角

        transBtn.setOnClickListener(v -> {
            // 注入 JS 获取页面纯文本，然后弹窗显示
            webView.evaluateJavascript("(function(){return document.body.innerText;})();", value -> {
                if (value != null && value.length() > 2) {
                    // 去掉前后引号
                    String text = value.substring(1, value.length() - 1)
                                       .replace("\\n", "\n")
                                       .replace("\\\"", "\"");
                    
                    // 这里我们用最简单粗暴的方式：直接弹窗显示原文
                    // 真正的翻译需要接入像 DeepL 或者 有道 的公开 API
                    runOnUiThread(() -> {
                        new AlertDialog.Builder(MainActivity.this)
                            .setTitle("📄 当前页面内容 (翻译接口需自行填入)")
                            .setMessage(text)
                            .setPositiveButton("确定", null)
                            .show();
                    });
                }
            });
        });

        root.addView(webView, new android.widget.FrameLayout.LayoutParams(-1, -1));
        root.addView(transBtn, params);
        
        setContentView(root);
    }

    @Override
    public void onBackPressed() {
        if (webView.canGoBack()) { webView.goBack(); } else { super.onBackPressed(); }
    }
}
