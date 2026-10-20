package com.github.pro.max;

import android.app.Activity;
import android.app.AlertDialog;
import android.net.http.SslError;
import android.os.Bundle;
import android.webkit.*;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends Activity {
    private WebView webView;
    private List<String> ipList = Arrays.asList(
        "140.82.112.4", "140.82.113.4", "140.82.114.4",
        "140.82.116.3", "20.205.243.166", "20.205.243.168"
    );
    private int currentIpIndex = 0;
    private String currentIp = ipList.get(0);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        
        // 完全开启浏览器原生属性
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
        settings.setSupportMultipleWindows(true);
        settings.setJavaScriptCanOpenWindowsAutomatically(true);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        
        // 使用标准的移动端 UA
        settings.setUserAgentString("Mozilla/5.0 (Linux; Android 13; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36");

        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.setAcceptThirdPartyCookies(webView, true);
        cookieManager.flush();

        webView.setWebChromeClient(new WebChromeClient());

        webView.setWebViewClient(new WebViewClient() {
            // 1. 强制忽略 SSL 证书错误，解决用 IP 访问时的证书不匹配白屏
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed();
            }

            // 2. 拦截主框架的 URL 跳转，把域名替换成 IP
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url.contains("github.com")) {
                    String newUrl = url.replace("github.com", currentIp);
                    view.loadUrl(newUrl);
                    return true;
                }
                return false;
            }

            // 3. 当页面加载完成后，利用 JS 把 DOM 里残留的绝对链接统统换成 IP
            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                String js = "javascript:(function() {" +
                        "var ip = '" + currentIp + "';" +
                        "var links = document.querySelectorAll('a');" +
                        "for(var i=0; i<links.length; i++) {" +
                        "  if(links[i].href && links[i].href.includes('github.com')) {" +
                        "    links[i].href = links[i].href.replace('github.com', ip);" +
                        "  }" +
                        "}" +
                        "})()";
                view.evaluateJavascript(js, null);
            }

            // 4. 加载失败时切换下一个 IP，彻底解决卡死
            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (request.isForMainFrame()) {
                    if (currentIpIndex < ipList.size() - 1) {
                        currentIpIndex++;
                        currentIp = ipList.get(currentIpIndex);
                        view.stopLoading();
                        view.loadUrl("https://" + currentIp);
                    } else {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("连接超时")
                                .setMessage("当前所有节点均不可用，请检查网络。")
                                .setPositiveButton("重试", (dialog, which) -> {
                                    currentIpIndex = 0;
                                    currentIp = ipList.get(0);
                                    view.stopLoading();
                                    view.loadUrl("https://" + currentIp);
                                })
                                .setNegativeButton("退出", (dialog, which) -> finish())
                                .show();
                    }
                }
            }
        });
        
        // 5. 直接加载 IP，让 Chromium 自己搞定一切，绝不去抓取
        webView.loadUrl("https://" + currentIp);
        setContentView(webView);
    }

    // 6. 返回键直接处理，不拦截，直接放行
    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
