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
        
        // 开启所有浏览器该有的特性
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
            // 【核心1】强制忽略 SSL 证书错误，解决用 IP 访问时的证书不匹配白屏
            @Override
            public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                handler.proceed();
            }

            // 【核心2】只拦截主框架的 URL 跳转，把 github.com 替换成 IP
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url.contains("github.com")) {
                    String newUrl = url.replace("github.com", currentIp);
                    view.loadUrl(newUrl);
                    return true;
                }
                return false;
            }

            // 【核心3】当页面加载完成后，注入 JS 强制替换所有内部绝对路径
            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                // 利用 JS 把 DOM 里残留的 github.com 链接统统换成 IP
                String js = "javascript:(function() {" +
                        "var ip = '" + currentIp + "';" +
                        "var links = document.querySelectorAll('a');" +
                        "for(var i=0; i<links.length; i++) {" +
                        "  if(links[i].href.includes('github.com')) {" +
                        "    links[i].href = links[i].href.replace('github.com', ip);" +
                        "  }" +
                        "}" +
                        "})()";
                view.evaluateJavascript(js, null);
            }

            // 加载失败时切换下一个 IP
            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (request.isForMainFrame()) {
                    if (currentIpIndex < ipList.size() - 1) {
                        currentIpIndex++;
                        currentIp = ipList.get(currentIpIndex);
                        webView.loadUrl("https://" + currentIp);
                    } else {
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("连接超时")
                                .setMessage("当前所有节点均不可用，请检查网络。")
                                .setPositiveButton("重试", (dialog, which) -> {
                                    currentIpIndex = 0;
                                    currentIp = ipList.get(0);
                                    webView.loadUrl("https://" + currentIp);
                                })
                                .setNegativeButton("退出", (dialog, which) -> finish())
                                .show();
                    }
                }
            }
        });
        
        // 直接用 IP 加载，让 WebView 自己去解析
        webView.loadUrl("https://" + currentIp);
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
